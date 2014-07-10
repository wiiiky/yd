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
#include "wio.h"
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <pthread.h>
#include <string.h>
#include <signal.h>
#include <termios.h>
#include <sys/types.h>
#include <sys/stat.h>

int w_read(int fd, void *buf, unsigned int count)
{
    int ret;
  AGAIN:
    errno = 0;
    ret = read(fd, buf, count);
    if (ret < 0 && (errno == EINTR || errno == EAGAIN)) {
        /* interrupted system call or try again */
        goto AGAIN;
    }
    return ret;
}

/* */
int w_readn(int fd, char *buf, unsigned int count)
{
    if (count == 0) {
        return -1;
    }

    int left = count - 1;
    int readn = 0;
    while (left > 0) {
        int n = w_read(fd, buf + readn, left);
        if (n <= 0) {           /* error or EOF */
            break;
        }
        left -= n;
        readn += n;
    }
    buf[readn] = '\0';
    return readn;
}

int w_write(int fd, void *buf, unsigned int count)
{
    int ret;
  AGAIN:
    errno = 0;
    ret = write(fd, buf, count);
    if (ret < 0 && (errno == EINTR || errno == EAGAIN)) {
        /* interrupted system call or try again */
        goto AGAIN;
    }
    return ret;
}

int w_writen(int fd, char *buf, unsigned int count)
{
    int left = count;
    int writen = 0;
    while (left > 0) {
        int n = w_write(fd, buf + writen, left);
        if (n <= 0) {           /* error */
            break;
        }
        left -= n;
        writen += n;
    }
    return writen;
}

static int w_is_fd_type_internal(int fd, int mode)
{
    struct stat statbuf;
    int ret = fstat(fd, &statbuf);
    if (ret != 0) {
        return 0;
    }
    if (((statbuf.st_mode & S_IFMT) == (mode_t) mode)) {
        return 1;
    }
    return 0;
}

int w_is_fd_socket(int fd)
{
    return w_is_fd_type_internal(fd, S_IFSOCK);
}

int w_is_fd_fifo(int fd)
{
    return w_is_fd_type_internal(fd, S_IFIFO);
}

/************************ READLINE ***********************************/
typedef struct {
    unsigned int size;          /* total size */
    unsigned int start;         /* the start position of buffer */
    unsigned int len;           /* valid buffer size */
    char *buf;
} ReadlineBuf;

#define DEFAULT_BUFSIZE (4096)

/*
 * initialize the buffer
 */
static ReadlineBuf *readline_buf_init()
{
    ReadlineBuf *buf = (ReadlineBuf *) malloc(sizeof(ReadlineBuf));
    buf->size = DEFAULT_BUFSIZE;
    buf->buf = malloc(sizeof(char) * DEFAULT_BUFSIZE);
    buf->len = 0;
    buf->start = 0;
    return buf;
}

/*
 * free the buffer
 */
static void readline_buf_free(ReadlineBuf * buf)
{
    if (buf == NULL) {
        return;
    }
    free(buf->buf);
    free(buf);
}

/*
 * move data to the header of buffer.
 */
static void readline_buf_forward(ReadlineBuf * lbuf)
{
    if (lbuf->start <= 0) {
        return;
    }

    int i, j;
    int total = lbuf->start + lbuf->len;
    for (i = lbuf->start, j = 0; i < total; i++, j++) {
        lbuf->buf[j] = lbuf->buf[i];
    }
    lbuf->len = lbuf->len;
    lbuf->start = 0;
}

/*
 * enlarge buffer
 */
static void readline_buf_enlarge(ReadlineBuf * buf)
{
    buf->size = buf->size << 1; /* double */
    buf->buf = realloc(buf->buf, buf->size);
}

static void readline_buf_make_space(ReadlineBuf * lbuf)
{
    if (lbuf->len >= lbuf->size - 1) {
        readline_buf_enlarge(lbuf);
    } else if (lbuf->len >= lbuf->size / 2
               || lbuf->start >= lbuf->size / 2) {
        /* if current length is larger than half of total size 
         * or the current start position is larger than half of total size
         * we move the data to the header of buffer
         */
        readline_buf_forward(lbuf);
    }
}


static void readline_buf_copydata(ReadlineBuf * lbuf, void *buf,
                                  unsigned int count)
{
    strncpy(buf, lbuf->buf + lbuf->start, count);
    ((char *) buf)[count] = '\0';
    lbuf->start += count;
    lbuf->len -= count;
}

static char *readline_buf_findline(ReadlineBuf * lbuf)
{
    int i;
    int total = lbuf->start + lbuf->len;
    for (i = lbuf->start; i < total; i++) {
        if (lbuf->buf[i] == '\n') {
            return lbuf->buf + i;
        }
    }
    return NULL;
}

/*
 * copy a line from ReadlineBuf into buf,
 * return copied size if success,
 * return 0 if fail
 */
static int readline_buf_copyline(ReadlineBuf * lbuf, void *buf,
                                 unsigned int count)
{
    char *line = readline_buf_findline(lbuf);
    if (line == NULL) {
        return 0;
    }
    unsigned int len = line - (lbuf->buf + lbuf->start) + 1;
    unsigned int move = len < count ? len : count - 1;
    readline_buf_copydata(lbuf, buf, move);
    return move;
}

static unsigned int readline_buf_copyall(ReadlineBuf * lbuf, void *buf,
                                         unsigned int count)
{
    unsigned int move = lbuf->len < count ? lbuf->len : count - 1;
    readline_buf_copydata(lbuf, buf, move);
    return move;
}


/* Thread-Specific Data */
static pthread_key_t pkey;
static pthread_once_t pkey_once = PTHREAD_ONCE_INIT;

/* destroy the Thread-Specific Data */
static void destroy_pthread_data(void *ptr)
{
    ReadlineBuf *buf = (ReadlineBuf *) ptr;
    readline_buf_free(buf);
}

static void create_pthread_key(void)
{
    (void) pthread_key_create(&pkey, destroy_pthread_data);
}

static ReadlineBuf *get_pthread_data(void)
{
    ReadlineBuf *lbuf = NULL;
    (void) pthread_once(&pkey_once, create_pthread_key);

    if ((lbuf = (ReadlineBuf *) pthread_getspecific(pkey)) == NULL) {
        /* the first time, create Pthread-Specific Data */
        lbuf = readline_buf_init();
        (void) pthread_setspecific(pkey, lbuf);
    }
    return lbuf;
}

int w_readline(int fd, void *buf, unsigned int count)
{
    if (count <= 0) {
        return 0;
    }

    ReadlineBuf *lbuf = get_pthread_data();

    int rd;
    if ((rd = readline_buf_copyline(lbuf, buf, count)) > 0) {
        return rd;
    }

    readline_buf_make_space(lbuf);
    rd = 0;
    while ((rd =
            w_read(fd, lbuf->buf + lbuf->start + lbuf->len,
                   lbuf->size - (lbuf->len + lbuf->start))) >= 0) {
        if (rd == 0) {
            /* EOF, return all buffer */
            return readline_buf_copyall(lbuf, buf, count);
        }
        lbuf->len += rd;
        if ((rd = readline_buf_copyline(lbuf, buf, count)) > 0) {
            return rd;
        }
        readline_buf_make_space(lbuf);
    }
    /* error occurs */
    return rd;
}

int w_readline_buffer(void *buf, unsigned int count)
{
    if (count <= 0) {
        return 0;
    }

    ReadlineBuf *lbuf = get_pthread_data();

    return readline_buf_copyall(lbuf, buf, count);
}

/***********************END of READLINE ***********************/

#define MAX_PASS_LEN    (128)
char *w_readpass(const char *prompt)
{
    char buf[MAX_PASS_LEN];
    char *ptr;
    sigset_t sig, osig;
    struct termios ts, ots;
    FILE *fp;
    int c;

    if ((fp = fopen(ctermid(NULL), "r+")) == NULL) {
        return NULL;
    }
    setbuf(fp, NULL);

    sigemptyset(&sig);
    sigaddset(&sig, SIGINT);    /* CTRL_C */
    sigaddset(&sig, SIGTSTP);   /* CTRL_Z */
    if (sigprocmask(SIG_BLOCK, &sig, &osig) != 0) { /* save mask */
        return NULL;
    }

    int fd = fileno(fp);

    tcgetattr(fd, &ts);         /* save TTY state */
    ots = ts;
    ts.c_lflag &= ~(ECHO | ECHOE | ECHOK | ECHONL);
    if (tcsetattr(fd, TCSAFLUSH, &ts) != 0) {
        sigprocmask(SIG_SETMASK, &osig, NULL);
        return NULL;
    }
    fputs(prompt, fp);

    ptr = buf;
    while ((c = getc(fp)) != EOF && c != '\n' && c != '\r') {
        if (ptr < &buf[MAX_PASS_LEN]) {
            *ptr = c;
            ptr++;
        }
    }
    *ptr = '\0';
    putc('\n', fp);             /* echo a newline */

    tcsetattr(fd, TCSAFLUSH, &ots);
    sigprocmask(SIG_SETMASK, &osig, NULL);  /* restore signal mask */
    fclose(fp);

    return strdup(buf);
}
