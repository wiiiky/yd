/*
 * proc_net.c
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
#include "proc_net.h"
#include "wio.h"
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/stat.h>


static int tcpfd = -1;

static int proc_net_tcp_open_real()
{
    struct stat statbuf;
    if (tcpfd >= 0 && !fstat(tcpfd, &statbuf)) {
        lseek(tcpfd, 0, SEEK_SET);
        return tcpfd;
    }
    tcpfd = open(TCPSTAT_FILE, O_RDONLY);
    return tcpfd;
}

static ProcNetTcpEntry *proc_net_tcp_entry_alloc()
{
    ProcNetTcpEntry *tcp =
        (ProcNetTcpEntry *) malloc(sizeof(ProcNetTcpEntry));
    tcp->sl = NULL;
    tcp->local_address = NULL;
    tcp->rem_address = NULL;
    tcp->st = NULL;
    tcp->tx_rx_queue = NULL;
    tcp->tr_tm_when = NULL;
    tcp->retrnsmt = NULL;
    tcp->uid = NULL;
    tcp->timeout = NULL;
    tcp->inode = NULL;

    return tcp;
}

void proc_net_tcp_entry_free(void *data)
{
    ProcNetTcpEntry *tcp = (ProcNetTcpEntry *) data;
    free(tcp->sl);
    free(tcp->local_address);
    free(tcp->rem_address);
    free(tcp->st);
    free(tcp->tx_rx_queue);
    free(tcp->tr_tm_when);
    free(tcp->retrnsmt);
    free(tcp->uid);
    free(tcp->timeout);
    free(tcp->inode);
    free(tcp);
}

static void proc_net_parse_line_free(char **elements, unsigned int size)
{
    int i;
    for (i = 0; i < size; i++) {
        free(elements[i]);
    }
    free(elements);
}

static int is_validchar(char c)
{
    if (c >= '!' && c <= '~') {
        return 1;
    }
    return 0;
}

typedef struct {
    const char *pos;
    unsigned int len;
} LineItem;

static char **proc_net_parse_line(const char *line, unsigned int *length)
{
    GList *list = NULL;

    unsigned int size = 0;

    while (*line) {
        if (is_validchar(*line)) {
            size++;
            const char *ptr = line;
            unsigned int len = 0;
            while (is_validchar(*ptr)) {
                len++;
                ptr++;
            }
            LineItem *item = (LineItem *) malloc(sizeof(LineItem));
            item->pos = line;
            item->len = len;
            list = g_list_append(list, item);
            if (*ptr == '\0') {
                break;
            }
            line = ptr;
        }
        line++;
    }

    if (size == 0) {
        g_list_free_full(list, free);
        return NULL;
    }

    char **elements = (char **) malloc(sizeof(char *) * size);

    GList *ptr = list;
    int i = 0;
    while (ptr) {
        LineItem *item = (LineItem *) ptr->data;
        elements[i++] = strndup(item->pos, item->len);
        ptr = g_list_next(ptr);
    }
    g_list_free_full(list, free);
    *length = size;

    return elements;
}

static int _size = -1;

static int _sl = -1;
static int _local_address = -1;
static int _rem_address = -1;
static int _st = -1;
static int _tx_rx_queue = -1;
static int _tr_tm_when = -1;
static int _retrnsmt = -1;
static int _uid = -1;
static int _timeout = -1;
static int _inode = -1;

static int get_real_title_index(int i)
{
    int diff = 0;
    if (_tx_rx_queue >= 0) {
        diff++;
    }
    if (_tr_tm_when >= 0) {
        diff++;
    }
    return i - diff;
}

static void get_title_index(char **elements, unsigned int size)
{
    static int first = 1;
    if (first) {
        /* 在不同系统中各个属性的顺序可能不同，但是在同一个系统中不会改变，因此只要获取一次顺序 */
        first = 0;
        _size = size;
        int i;
        for (i = 0; i < size; i++) {
            if (strcmp(elements[i], "sl") == 0) {
                _sl = get_real_title_index(i);
            } else if (strcmp(elements[i], "local_address") == 0) {
                _local_address = get_real_title_index(i);
            } else if (strcmp(elements[i], "rem_address") == 0) {
                _rem_address = get_real_title_index(i);
            } else if (strcmp(elements[i], "st") == 0) {
                _st = get_real_title_index(i);
            } else if (strcmp(elements[i], "tx_queue") == 0) {
                _tx_rx_queue = get_real_title_index(i);
            } else if (strcmp(elements[i], "tr") == 0) {
                _tr_tm_when = get_real_title_index(i);
            } else if (strcmp(elements[i], "retrnsmt") == 0) {
                _retrnsmt = get_real_title_index(i);
            } else if (strcmp(elements[i], "uid") == 0) {
                _uid = get_real_title_index(i);
            } else if (strcmp(elements[i], "timeout") == 0) {
                _timeout = get_real_title_index(i);
            } else if (strcmp(elements[i], "inode") == 0) {
                _inode = get_real_title_index(i);
            }
        }
    }
}

static ProcNetTcpEntry *extract_entries(char **elements, unsigned int size)
{
    ProcNetTcpEntry *tcp = proc_net_tcp_entry_alloc();
    if (_sl >= 0 && _sl < size) {
        tcp->sl = strdup(elements[_sl]);
    }
    if (_local_address >= 0 && _local_address < size) {
        tcp->local_address = strdup(elements[_local_address]);
    }
    if (_rem_address >= 0 && _rem_address < size) {
        tcp->rem_address = strdup(elements[_rem_address]);
    }
    if (_st >= 0 && _st < size) {
        tcp->st = strdup(elements[_st]);
    }
    if (_tx_rx_queue >= 0 && _tx_rx_queue < size) {
        tcp->tx_rx_queue = strdup(elements[_tx_rx_queue]);
    }
    if (_tr_tm_when >= 0 && _tr_tm_when < size) {
        tcp->tr_tm_when = strdup(elements[_tr_tm_when]);
    }
    if (_retrnsmt >= 0 && _retrnsmt < size) {
        tcp->retrnsmt = strdup(elements[_retrnsmt]);
    }
    if (_uid >= 0 && _uid < size) {
        tcp->uid = strdup(elements[_uid]);
    }
    if (_timeout >= 0 && _timeout < size) {
        tcp->timeout = strdup(elements[_timeout]);
    }
    if (_inode >= 0 && _inode < size) {
        tcp->inode = strdup(elements[_inode]);
    }
    return tcp;
}

#define MAX_LINE_SIZE   (1024)

GList *proc_net_tcp_open()
{
    proc_net_tcp_open_real();

    char linebuf[MAX_LINE_SIZE];
    int n;

    GList *list = NULL;         /* NULL */

    unsigned int size;
    char **elements = NULL;
    /* first line */
    n = w_readline(tcpfd, linebuf, MAX_LINE_SIZE);
    if (n <= 0 || (elements = proc_net_parse_line(linebuf, &size)) == NULL) {
        return list;
    }
    get_title_index(elements, size);
    proc_net_parse_line_free(elements, size);

    /* entries */
    while ((n = w_readline(tcpfd, linebuf, MAX_LINE_SIZE)) > 0) {
        elements = proc_net_parse_line(linebuf, &size);
        if (elements) {
            ProcNetTcpEntry *tcp = extract_entries(elements, size);
            list = g_list_append(list, tcp);
            proc_net_parse_line_free(elements, size);
        }
    }


    return list;
}

void proc_net_tcp_close(GList * list)
{
    g_list_free_full(list, proc_net_tcp_entry_free);
}


int proc_net_tcp_entry_number(ProcNetTcpEntry * tcp)
{
    if (tcp->sl) {
        return atoi(tcp->sl);
    }
    return -1;
}

static int proc_net_tcp_entry_address(const char *address,
                                      uint32_t * addr, uint16_t * port)
{
    if (address == NULL) {
        return -1;
    }
    char *colon = strchr(address, ':');
    if (colon == NULL) {        /* 没有: 则认为是无效的 */
        return -1;
    }
    *addr = strtol(address, NULL, 16);
    *port = strtol(colon + 1, NULL, 16);
    return 0;
}

int porc_net_tcp_entry_local(ProcNetTcpEntry * tcp,
                             uint32_t * addr, uint16_t * port)
{
    return proc_net_tcp_entry_address(tcp->local_address, addr, port);
}

int proc_net_tcp_entry_remote(ProcNetTcpEntry * tcp, uint32_t * addr,
                              uint16_t * port)
{
    return proc_net_tcp_entry_address(tcp->rem_address, addr, port);
}
