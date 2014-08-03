#include <stdio.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <pthread.h>

/* 本程序只是打开一个或者多个监听端口 */

static void *listen_on_port(void *arg);


int main(int argc, char *argv[])
{
    if (argc < 2) {
        return -1;
    }

    int i;
    pthread_t tid;
    for (i = 1; i < argc; i++) {
        pthread_create(&tid, NULL, listen_on_port, argv[i]);
        pthread_detach(tid);
    }

    while (1) {
    }

    return 0;
}

static void *listen_on_port(void *arg)
{
    unsigned short port = atoi((char *) arg);

    char buf[1024];

    int sockfd = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);

    struct sockaddr_in addr;
    addr.sin_family = AF_INET;
    addr.sin_port = htons(port);
    addr.sin_addr.s_addr = htonl(INADDR_ANY);
    if (bind(sockfd, (struct sockaddr *) &addr, sizeof(addr))) {
        snprintf(buf, 1024, "Failed to bind port %u", port);
        perror(buf);
        return NULL;
    }
    if (listen(sockfd, 100)) {
        snprintf(buf, 1024, "Failed to listen on port %u", port);
        perror(buf);
        return NULL;
    }

    printf("listen on port %u\n", port);

    socklen_t len;
    while (accept(sockfd, (struct sockaddr *) &addr, &len) > 0) {
        printf("%u\n", port);
    }

    return NULL;
}
