/*
 * sudo.h
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
#ifndef __YD_SUDO_H__
#define __YD_SUDO_H__

#include <inttypes.h>


/*
 * 判断是否具有相关权限
 * 具有返回1，否则返回0
 */
int yd_have_capabilities(uint32_t cap);
/*
 * 用sudo执行
 */
void yd_sudo(int argc, char **argv);

#endif
