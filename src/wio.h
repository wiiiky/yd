/*
 * Copyright (C) 2014  Wiky L
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 * 
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 * 
 * You should have received a copy of the GNU Lesser General Public
 * License along with main.c; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor Boston, MA 02110-1301,  USA
 */
#ifndef __W_WIO_H__
#define __W_WIO_H__

/*
 * wraper of read()
 */
int w_read(int fd, void *buf, unsigned int count);

/*
 * @description: read count bytes from given file descriptor,
 *              unless EOF or error occurs.
 * 
 * @param fd: file descriptor
 * @param buf: buffer
 * @param count: the byte size of buffer
 * 
 *              Notice that buf will always be null-terminated,
 *                      which means that only count-1 bytes will be read at max.
 *                      the given count must be larger than 0.
 *                      If count equals 1, just set buf[0]='\0' and return, no data will be read. 
 */
int w_readn(int fd, char *buf, unsigned int count);

/*
 * wraper of write() 
 */
int w_write(int fd, void *buf, unsigned int count);

/*
 * @description: writes count bytes to given file descriptor,
 *              unless error occurs.
 * 
 * @param fd: file descriptor
 * @param buf: buffer
 * @param count: the byte to write
 * 
 */
int w_writen(int fd, char *buf, unsigned int count);

/*
 * @description: checks if fd is a socket or not
 * 
 * @return: if fd is socket, return 1. Otherwise 0.
 */
int w_is_fd_socket(int fd);

/*
 * @description: check if fd is a FIFO descriptor or not
 * 
 * @return: if fd is FIFO, return 1. Otherwise 0;
 */
int w_is_fd_fifo(int fd);


/*
 * @description: reads a line from file
 *				this function will cache data
 *
 * @param fd: the file descriptor
 * @param: the buffer 
 * @param: the buffer size
 *
 * @return: the size read
 */
int w_readline(int fd, void *buf, unsigned int count);



/*
 * @description: this function copys count bytes of data in readline buffer into buf
 * 
 * @return: the size of data that copied
 */
int w_readline_buffer(void *buf, unsigned int count);

/*
 * @description: reads input from terminal without echoing
 * 
 * @return: the pointer to the new allocated buffer,or NULL on error
 */
char *w_readpass(const char *prompt);


#endif
