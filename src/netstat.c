/*
 * netstat.c
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

#include "netstat.h"
#include "wio.h"
#include <unistd.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <stdio.h>
#include <stdlib.h>

static int tcpfd = -1;

static int ns_stat_open_tcp()
{
    struct stat statbuf;
    if (tcpfd >= 0 && !fstat(tcpfd, &statbuf)) {
        lseek(tcpfd, 0, SEEK_SET);
        return tcpfd;
    }
    tcpfd = open(TCPSTAT_FILE, O_RDONLY);
    return tcpfd;
}

static void ns_stat_list_free(void *data)
{
    WList *list = (WList *) data;
    w_list_free_full(list, free);
}

static void ns_stat_title_free(void *data)
{
    free(data);
}

#define LINE_BUF_SIZE   (1024)

TcpStat *ns_stat_tcp_new()
{
    TcpStat *stat = w_hash_table_new(10,
                                     w_str_hash,
                                     w_str_equal,
                                     ns_stat_title_free,
                                     ns_stat_list_free);
    ns_stat_open_tcp();
    char buf[LINE_BUF_SIZE];
    int n;
    while ((n = w_readline(tcpfd, buf, LINE_BUF_SIZE)) > 0) {
        printf("%s", buf);
    }
    return stat;
}

void ns_stat_tcp_free(TcpStat * stat)
{
    w_hash_table_free(stat);
}
