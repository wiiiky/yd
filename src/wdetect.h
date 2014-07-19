/*
 * wdetect.h
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
#ifndef __W_DETECT_H__
#define __W_DETECT_H__

#include <pcap.h>
#include <gtk/gtk.h>
#include <inttypes.h>

/* 一个syn+ack数据包的信息 */
typedef struct {
    uint32_t addr;              /* 地址 */
    uint32_t seq;               /* 序号 */
    uint32_t seq_ack;           /* 相应序号,这个不重要 */
} SynInfo;


/* 一个端口的信息,动态处理syn包 */
typedef struct {
    uint16_t port;              /* 端口 */
    /*SynInfo syn[100];     *//* 简化处理，用了数组 */
    GList *syn;                 /* 链表 */
} PortInfo;

/* 抓取数据包的回调函数 */
void capture_packet(unsigned char *arg, const struct pcap_pkthdr *pkthdr,
                    const unsigned char *packet);

/* 
 * @description 打开指定的网络接口，并设置数据包过滤规则
 * @param iface 网络接口名，一般有线网卡是eth0，无线网卡是wlan0，在进行测试的时候使用回环地址，因此是lo
 * @param bpf, Berkeley Packet Filter， 数据包过滤规则
 * @return 返回pcap_t的指针
 * */
pcap_t *capture_live(const char *iface, const char *bpf);


void yd_detect_run(GAsyncQueue * queue);

/*
 * @description 检测攻击的线程
 */
void *yd_detect_thread(void *arg);


#endif
