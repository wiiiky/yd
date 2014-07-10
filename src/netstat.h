/*
 * netstat.h
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
#ifndef __YD_NETSTAT_H__
#define __YD_NETSTAT_H__


#include "whashtable.h"


#define TCPSTAT_FILE    "/proc/net/tcp"

typedef WHashTable TcpStat;



TcpStat *ns_stat_tcp_new();

void ns_stat_tcp_free(TcpStat * stat);

#endif
