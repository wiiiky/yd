#include <stdio.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <arpa/inet.h>

/* 本程序只是打开一个监听端口 */


int main(int argc, char *argv[])
{
	int sockfd=socket(AF_INET,SOCK_STREAM,IPPROTO_TCP);

	struct sockaddr_in addr;
	addr.sin_family=AF_INET;
	addr.sin_port=htons(1234);
	addr.sin_addr.s_addr=htonl(INADDR_ANY);
	bind(sockfd,(struct sockaddr*)&addr,sizeof(addr));
	listen(sockfd,100);

	printf("listen on port 1234\n");

	socklen_t len;
	while(accept(sockfd,(struct sockaddr*)&addr,&len)>0){
		printf("!\n");
	}
	return 0;
}
