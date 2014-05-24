#include <stdlib.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <inttypes.h>
#include "wlist.h"


/* 一个syn+ack数据包的信息 */
typedef struct {
	uint32_t addr;		/* 地址 */
	unsigned int seq;	/* 序号 */
	unsigned int seq_ack;	/* 相应序号,这个不重要 */
}SynInfo;


/* 一个端口的信息,动态处理syn包 */
typedef struct {
	uint16_t port;	/* 端口 */
	/*SynInfo syn[100];		*//* 简化处理，用了数组 */
	WList *syn;			/* 链表 */
}PortInfo;
