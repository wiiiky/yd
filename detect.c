#include "detect.h"


/* 抓取数据包的回调函数 */
void capture_packet(unsigned char *arg,const struct pcap_pkthdr *pkthdr,
			const unsigned char *packet)
{
	/* 链路层首部 */
	struct ethhdr *eth=(struct ethhdr*)packet;
	if(pkthdr->len<=14){	/* 简单的错误检测，一般不会有错 */
		return;
	}

	/* IP 首部 */
	struct iphdr *ip=(struct iphdr *)(packet+14);
	if(ip->protocol!=IPPROTO_TCP){
		return ;
	}
	/* TCP 首部, *IP首部的长度表示的是32bit的倍数，乘4转化为字节 */
	struct tcphdr *tcp=(struct tcphdr*)(((unsigned char *)ip)+ip->ihl*4);
	if(tcp->syn&&tcp->ack)
	printf("%u:%u\n",ntohs(tcp->source),ntohs(tcp->dest));
}

/* 
 * @description 打开指定的网络接口，并设置数据包过滤规则
 * @param iface 网络接口名，一般有线网卡是eth0，无线网卡是wlan0，在进行测试的时候使用回环地址，因此是lo
 * @param bpf, Berkeley Packet Filter， 数据包过滤规则
 * @return 返回pcap_t的指针
 * */
pcap_t *capture_live(const char *iface,const char *bpf)
{
	char errbuf[PCAP_ERRBUF_SIZE];
	
	if(iface==NULL){
		return NULL;
	}

	/*
	 * 第一个参数, 网络接口的字符串
	 * 第二个参数, 表示对于每个数据包要抓取的大小，65535是最大值
	 * 第三个参数, 表示是否打开混杂模式，指定0表示不打开
	 * 第四个参数, 表示等待数据包的超时（毫秒），0表示不超时，一直等待。
	 * 第五个参数, 出错信息
	 */
	pcap_t *device=pcap_open_live(iface,65535,0,0,errbuf);
	if(!device){
		return NULL;
	}

	/*
	 * 编译过滤规则并设置
	 *
	 * 第三个参数表示优化表达式
	 * 第四个设置为0就好
	 */

	struct bpf_program filter;
	pcap_compile(device,&filter,bpf,1,0);
	pcap_setfilter(device,&filter);

	return device;
}
