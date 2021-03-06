/*
 * wdetect.c
 *
 * Copyright (C) 2014 - Wiky L
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program. If not, see <http://www.gnu.org/licenses/>.
 */
#include "wdetect.h"

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <time.h>
#include <errno.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <sys/ioctl.h>
#include <net/if.h>
#include <arpa/inet.h>
#include <pcap.h>
#include <linux/if_ether.h>
#include <linux/ip.h>
#include <linux/tcp.h>

/* 所有端口的链表 */
static GList *ports = NULL;

/* 端口查找的函数 */
static gint port_compare(gconstpointer a, gconstpointer b);

/* 获取本机所有的IP地址，由family指定地址域AF_INET/AF_INET6 */
static GList *get_ips(int family);

/* 释放IP地址的链表 */
static void free_ips(GList * list);

/*
 * 如果addr是本地地址，返回1，否则返回0
 */
static int is_localaddr(GList * list, uint32_t addr);

/*
 * @description 检测攻击的线程
 */
static void *yd_detect_thread(void *arg);

/* 包裹地址和端口号，用于g_list_compare_custom */
typedef struct {
    uint32_t addr;
    uint16_t port;
} GListCompareStruct;

typedef struct {
    GList *ips;
    GAsyncQueue *queue;
} CapturePacketArgument;

/* 抓取数据包的回调函数 */
void capture_packet(unsigned char *data, const struct pcap_pkthdr *pkthdr,
                    const unsigned char *packet)
{
    CapturePacketArgument *arg = (CapturePacketArgument *) data;
    GAsyncQueue *queue = arg->queue;
    GList *ips = arg->ips;
    /* 链路层首部 */
    packet = packet + 2;        /* 使用any抓取的包前面加了两个字节 表示类型？ ref http://seclists.org/tcpdump/2009/q4/73 */
    if (pkthdr->len <= 14) {    /* 简单的错误检测，一般不会有错 */
        return;
    }

    /* IP 首部 */
    struct iphdr *ip = (struct iphdr *) (packet + 14);
    uint32_t saddr = ip->saddr; /* 源IP地址 */
    uint32_t daddr = ip->daddr; /* 目的IP地址 */

    GListCompareStruct compare;
    if (ip->protocol != IPPROTO_TCP) {
        return;
    }
    /* TCP 首部, *IP首部的长度表示的是32bit的倍数，乘4转化为字节 */
    struct tcphdr *tcp =
        (struct tcphdr *) (((unsigned char *) ip) + ip->ihl * 4);
    uint16_t sport = ntohs(tcp->source);    /* 被链接（攻击）的端口号 */

    compare.addr = saddr;
    compare.port = sport;

    if (tcp->syn && tcp->ack && is_localaddr(ips, saddr)) {
        /* 本机发出的SYN+ACK响应 */
        SynInfo *sinfo = malloc(sizeof(SynInfo));
        sinfo->addr = daddr;
        sinfo->seq = ntohl(tcp->seq);
        sinfo->seq_ack = ntohl(tcp->ack_seq);

        GList *l = g_list_find_custom(ports, (gconstpointer) & compare,
                                      port_compare);
        if (l == NULL) {        /* 这个端口第一次 */
            PortInfo *pinfo = malloc(sizeof(PortInfo));
            pinfo->localaddr = saddr;
            pinfo->port = sport;
            pinfo->syn = g_list_append(NULL, sinfo);
            ports = g_list_append(ports, pinfo);
        } else {
            PortInfo *pinfo = (PortInfo *) l->data;
            pinfo->syn = g_list_append(pinfo->syn, sinfo);
            if (g_list_length(pinfo->syn) >= 200) {
                /* 如果没有响应的数量超过200，则认为遭到了攻击 */
                g_async_queue_lock(queue);
                g_async_queue_push_unlocked(queue, (gpointer) (long)
                                            pinfo->localaddr);
                g_async_queue_push_unlocked(queue,
                                            (gpointer) (long) pinfo->port);
                g_async_queue_unlock(queue);
                g_list_free_full(pinfo->syn, free); /* 释放内存，避免溢出 */
                pinfo->syn = NULL;
            }
        }
    } else if (!tcp->syn && tcp->ack && is_localaddr(ips, daddr)) {
        /* 本机收到的ACK */
        GList *l = g_list_find_custom(ports, (gconstpointer) & compare,
                                      port_compare);
        if (l) {
            PortInfo *pinfo = (PortInfo *) l->data;
            GList *slist = pinfo->syn;
            while (slist) {
                SynInfo *sinfo = (SynInfo *) slist->data;
                if (sinfo->seq + 1 == tcp->ack_seq && sinfo->addr == saddr) {
                    /* 捕获的数据包是一个已知的响应 */
                    pinfo->syn = g_list_remove(pinfo->syn, sinfo);
                    free(sinfo);
                    break;
                }
                slist = g_list_next(slist);
            }
        }
    }
}

/* 
 * @description 打开指定的网络接口，并设置数据包过滤规则
 * @param iface 网络接口名，一般有线网卡是eth0，无线网卡是wlan0，在进行测试的时候使用回环地址，因此是lo
 * @param bpf, Berkeley Packet Filter， 数据包过滤规则
 * @return 返回pcap_t的指针
 * */
pcap_t *capture_live(const char *iface, const char *bpf)
{
    char errbuf[PCAP_ERRBUF_SIZE];

    if (iface == NULL) {
        return NULL;
    }

    /*
     * 第一个参数, 网络接口的字符串，any或者NULL表示所有接口
     * 第二个参数, 表示对于每个数据包要抓取的大小，65535是最大值
     * 第三个参数, 表示是否打开混杂模式，指定0表示不打开
     * 第四个参数, 表示等待数据包的超时（毫秒），0表示不超时，一直等待。
     * 第五个参数, 出错信息
     */
    pcap_t *device = pcap_open_live(iface, 65535, 0, 0, errbuf);
    if (!device) {
        return NULL;
    }

    /*
     * 编译过滤规则并设置
     *
     * 第三个参数表示优化表达式
     * 第四个设置为0就好
     */

    struct bpf_program filter;
    pcap_compile(device, &filter, bpf, 1, 0);
    pcap_setfilter(device, &filter);

    return device;
}

static struct sockaddr *sockaddr_copy(struct sockaddr *addr)
{
    int len;
#ifdef HAVE_SOCKADDR_SA_LEN
    len = MAX(sizeof(struct sockaddr), addr->sa_len);
#else
    if (addr->sa_family == AF_INET6) {
        len = sizeof(struct sockaddr_in6);
    } else {
        len = sizeof(struct sockaddr_in);
    }
#endif                          /* HAVE_SOCKADDR_SA_LEN */
    struct sockaddr *ret = (struct sockaddr *) malloc(len);
    memcpy(ret, addr, len);
    return ret;
}

/* 获取本机所有的IP地址 */
static GList *get_ips(int family)
{
    struct ifconf ifc;
    GList *ips = NULL;

    int sockfd = socket(family, SOCK_DGRAM, 0);

    int lastlen = 0;
    int len = 32 * sizeof(struct ifreq);    /* 初始化缓冲区大小，表示本地最大有32个IP地址，这个大小一般不需要再分配了 */
    ifc.ifc_buf = NULL;
    while (TRUE) {
        /* 调用realloc(NULL,size)相当于malloc(size) */
        ifc.ifc_buf = realloc(ifc.ifc_buf, len);
        ifc.ifc_len = len;
        if (ioctl(sockfd, SIOCGIFCONF, &ifc) < 0) {
            if (errno != EINVAL || lastlen != 0) {
                /* ERROR */
                free(ifc.ifc_buf);
                return ips;
            }
        } else if (ifc.ifc_len == lastlen) {
            /* 成功，两次返回的长度是一致的，说明缓冲区是足够大的 */
            break;
        } else {
            lastlen = ifc.ifc_len;
        }
        /* 缓冲区不够大，继续分配 */
        len += 10 * sizeof(struct ifreq);
    }
    char *ptr = ifc.ifc_buf;
    /* 遍历struct ifreq数组 */
    while (ptr < ifc.ifc_buf + ifc.ifc_len) {
        struct ifreq *ifr = (struct ifreq *) ptr;
        ptr += sizeof(struct ifreq);

        if (ifr->ifr_addr.sa_family != family) {
            continue;           /* 忽略不符合要求的地址 */
        }

        struct ifreq ifrcopy = *ifr;
        ioctl(sockfd, SIOCGIFFLAGS, &ifrcopy);
        if ((ifrcopy.ifr_flags & IFF_UP) == 0) {
            continue;           /* 忽略不工作的接口 */
        }

        struct sockaddr *addr = sockaddr_copy(&(ifr->ifr_addr));
        ips = g_list_append(ips, addr);
    }

    free(ifc.ifc_buf);
    return ips;
}

/* 释放IP地址的链表 */
static void free_ips(GList * list)
{
    g_list_free_full(list, free);
}

static int is_localaddr(GList * list, uint32_t addr)
{
    GList *ptr = list;
    while (ptr) {
        struct sockaddr_in *localaddr = (struct sockaddr_in *) ptr->data;
        if (localaddr->sin_addr.s_addr == addr) {
            return 1;
        }
        ptr = g_list_next(ptr);
    }
    return 0;
}

static gint port_compare(gconstpointer a, gconstpointer b)
{
    const GListCompareStruct *compare = (const GListCompareStruct *) b;
    const PortInfo *pinfo = (const PortInfo *) a;
    if (pinfo->port == compare->port && pinfo->localaddr == compare->addr) {
        return 0;
    }
    return -1;
}

void yd_detect_run(GAsyncQueue * queue)
{
    g_async_queue_ref(queue);
    GThread *thread = g_thread_new("detect", yd_detect_thread, queue);
    g_thread_unref(thread);
}


static void *yd_detect_thread(void *data)
{
    GAsyncQueue *queue = (GAsyncQueue *) data;
    GList *ips = get_ips(AF_INET);
    /*
     * tcp[13]表示tcp首部的第十三个字节，网络字节序，对应的是标志字段
     *
     * tcp[13]==0x12表示捕获带有SYN和ACK标志的数据包，这种数据包是对SYN的响应
     * tcp[13]==0x10表示带有ACK的数据包，这种数据包是一般的TCP响应
     * 如果主机受到SYN链接，那么主机会返回SYN+ACK的数据包，也就是第一种，
     * 如果远程主机是正常链接，那么接下去还有一个ACK的确认包，
     * 如果远程主机是攻击性的，那么就不会有一个ACK的确认包。
     */
    pcap_t *pcap = capture_live("any", "tcp[13]==0x12 or tcp[13]==0x10");
    if (pcap == NULL) {
        fprintf(stderr, "Permission???\n");
        exit(EXIT_FAILURE);
    }

    CapturePacketArgument arg = { ips, queue };
    pcap_loop(pcap, -1, capture_packet, (unsigned char *) &arg);

    pcap_close(pcap);

    free_ips(ips);
    g_async_queue_unref(queue);

    return NULL;
}
