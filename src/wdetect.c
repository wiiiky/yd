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
#include <sys/socket.h>
#include <sys/types.h>
#include <sys/ioctl.h>
#include <net/if.h>
#include <arpa/inet.h>
#include <pcap.h>
#include <linux/if_ether.h>
#include <linux/ip.h>
#include <linux/tcp.h>
#include <inttypes.h>

/* 所有端口的链表 */
static GList *ports = NULL;
static uint32_t localaddr = 0;

/* 端口查找的函数 */
static int port_compare(void *a, void *b);


/* 获取本地IP地址 */
int get_iface_ip(struct sockaddr_in *ip, const char *iface);


/* 抓取数据包的回调函数 */
void capture_packet(unsigned char *arg, const struct pcap_pkthdr *pkthdr,
                    const unsigned char *packet)
{
    /* 链路层首部 */
    //struct ethhdr *eth = (struct ethhdr *) packet;
    if (pkthdr->len <= 14) {    /* 简单的错误检测，一般不会有错 */
        return;
    }

    /* IP 首部 */
    uint32_t saddr;
    uint32_t daddr;
    struct iphdr *ip = (struct iphdr *) (packet + 14);
    saddr = ntohl(ip->saddr);   /* 源IP地址 */
    daddr = ntohl(ip->daddr);   /* 目的IP地址 */
    if (ip->protocol != IPPROTO_TCP) {
        return;
    }
    /* TCP 首部, *IP首部的长度表示的是32bit的倍数，乘4转化为字节 */
    struct tcphdr *tcp =
        (struct tcphdr *) (((unsigned char *) ip) + ip->ihl * 4);
    uint16_t sport = ntohs(tcp->source);    /* 被链接（攻击）的端口号 */
    if (tcp->syn && tcp->ack && saddr == localaddr) {
        /* 本机发出的SYN+ACK响应 */
        SynInfo *sinfo = malloc(sizeof(SynInfo));
        sinfo->addr = daddr;
        sinfo->seq = ntohl(tcp->seq);
        sinfo->seq_ack = ntohl(tcp->ack_seq);

        GList *l =
            g_list_find_custom(ports, port_compare, (void *) (long) sport);
        if (l == NULL) {        /* 这个端口第一次 */
            PortInfo *pinfo = malloc(sizeof(PortInfo));
            pinfo->port = sport;
            pinfo->syn = g_list_append(NULL, sinfo);
            ports = g_list_append(ports, pinfo);
        } else {
            PortInfo *pinfo = (PortInfo *) l->data;
            pinfo->syn = g_list_append(pinfo->syn, sinfo);
            if (g_list_length(pinfo->syn) >= 200) {
                /* 如果没有响应的数量超过200，则认为遭到了攻击 */
                printf("Warning: port %u is under attack!!!\n",
                       pinfo->port);
                g_list_free_full(pinfo->syn, free); /* 释放内存，避免溢出 */
                pinfo->syn = NULL;
            }
        }
    } else if (!tcp->syn && tcp->ack && daddr == localaddr) {
        /* 本机受到的ACK */
        GList *l =
            g_list_find_custom(ports, port_compare, (void *) (long) sport);
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

    struct sockaddr_in addr;
    if (!get_iface_ip(&addr, iface)) {
        return NULL;
    }
    localaddr = ntohl(addr.sin_addr.s_addr);

    /*
     * 第一个参数, 网络接口的字符串
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


/* 获取本地IP地址,需要获取全部地址 TODO */
int get_iface_ip(struct sockaddr_in *ip, const char *iface)
{
    int fd;
    struct ifreq ifr;
    fd = socket(AF_INET, SOCK_DGRAM, 0);
    ifr.ifr_addr.sa_family = AF_INET;
    strncpy(ifr.ifr_name, iface, IFNAMSIZ - 1);
    int ret = ioctl(fd, SIOCGIFADDR, &ifr);
    if (ret != 0) {
        return 0;
    }
    close(fd);
    ip->sin_family = AF_INET;
    ip->sin_addr = ((struct sockaddr_in *) &ifr.ifr_addr)->sin_addr;
    return 1;
}

static int port_compare(void *a, void *b)
{
    uint16_t port = (uint16_t) (long) b;
    PortInfo *pinfo = (PortInfo *) a;
    if (pinfo->port == port) {
        return 0;
    }
    return -1;
}
