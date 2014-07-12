/*
 * proc_net.h
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
#ifndef __YD_PROC_NET_H__
#define __YD_PROC_NET_H__

/*
 * 解析/proc/net下的文件信息
 * 主要是/proc/net/tcp和/proc/net/udp
 */

#include <glib.h>


#define TCPSTAT_FILE "/proc/net/tcp"


typedef struct {
    char *sl;                   /* sl */
    char *local_address;        /* local_address */
    char *rem_address;          /* rem_address */
    char *st;                   /* st */
    char *tx_rx_queue;          /* tx_queue rx_queue */
    char *tr_tm_when;           /* tr tm->when */
    char *retrnsmt;             /* retrnsmt */
    char *uid;                  /* uid */
    char *timeout;              /* timeout */
    char *inode;                /* inode */
    /* TODO */
} ProcNetTcpEntry;

/*
 * 读取/proc/net/tcp，并解析得到一个当前打开的TCP端口列表
 * 列表的元素为ProcNetTcp
 */
GList *proc_net_tcp_open();

void proc_net_tcp_close(GList * list);

int proc_net_tcp_entry_number(ProcNetTcpEntry * tcp);



#endif
