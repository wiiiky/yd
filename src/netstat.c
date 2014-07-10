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
#include <string.h>
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

static void ns_stat_line_free(char **eles, unsigned int size)
{
    free(eles);
}

static void ns_stat_line_free_full(char **eles, unsigned int size)
{
    int i;
    for (i = 0; i < size; i++) {
        free(eles[i]);
    }
    free(eles);
}

static char **ns_stat_parse_line(const char *line, unsigned int *length)
{
    WList *list = w_list_new();

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
            list = w_list_append(list, item);
            if (*ptr == '\0') {
                break;
            }
            line = ptr;
        }
        line++;
    }

    if (size == 0) {
        w_list_free_full(list, free);
        return NULL;
    }

    char **elements = (char **) malloc(sizeof(char *) * size);

    WList *ptr = list;
    int i = 0;
    while (ptr) {
        LineItem *item = (LineItem *) w_list_data(ptr);
        elements[i++] = strndup(item->pos, item->len);
        ptr = w_list_next(ptr);
    }
    w_list_free_full(list, free);
    *length = size;

    return elements;
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
    /* first line: TITLE */
    if ((n = w_readline(tcpfd, buf, LINE_BUF_SIZE)) <= 0) {
        return stat;            /* return an empty TcpStat */
    }
    unsigned int size;
    int i;
    char **titles = ns_stat_parse_line(buf, &size);
    if (titles == NULL) {
        return stat;
    }
    /* initialize lists */
    WList **lists = (WList **) malloc(sizeof(WList *) * size);
    for (i = 0; i < size; i++) {
        lists[i] = NULL;
    }

    while ((n = w_readline(tcpfd, buf, LINE_BUF_SIZE)) > 0) {
        unsigned int len;
        char **eles = ns_stat_parse_line(buf, &len);
        if (eles) {
            if (len >= size) {
                for (i = 0; i < size; i++) {
                    lists[i] = w_list_append(lists[i], eles[i]);
                }
                ns_stat_line_free(eles, len);
            } else {
                ns_stat_line_free_full(eles, len);
            }
        }
    }

    for (i = 0; i < size; i++) {
        w_hash_table_insert(stat, titles[i], lists[i]);
    }

    ns_stat_line_free(titles, size);
    free(lists);
    return stat;
}

void ns_stat_tcp_free(TcpStat * stat)
{
    w_hash_table_free(stat);
}
