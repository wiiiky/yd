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
#include <inttypes.h>


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


void proc_net_tcp_entry_free(void *tcp);

int proc_net_tcp_entry_number(ProcNetTcpEntry * tcp);

int porc_net_tcp_entry_local(ProcNetTcpEntry * tcp, uint32_t * addr,
                             uint16_t * port);

int proc_net_tcp_entry_remote(ProcNetTcpEntry * tcp, uint32_t * addr,
                              uint16_t * port);

const gchar *make_address_with_port(char *buf, uint32_t size,
                                    uint32_t addr, uint16_t port);

/* 返回静态缓冲区的内容 */
const gchar *make_tcp_local_address_with_port(ProcNetTcpEntry * tcp);

const gchar *make_tcp_remote_address_with_port(ProcNetTcpEntry * tcp);

const gchar *make_tcp_state(ProcNetTcpEntry * tcp);

const gchar *make_tcp_recv_q(ProcNetTcpEntry * tcp);

const gchar *make_tcp_send_q(ProcNetTcpEntry * tcp);

const gchar *make_tcp_uid(ProcNetTcpEntry * tcp);

const gchar *make_tcp_uname(ProcNetTcpEntry * tcp);

const gchar *make_tcp_uhome(ProcNetTcpEntry * tcp);

#endif
