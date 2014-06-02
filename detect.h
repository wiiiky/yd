#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <net/if.h>
#include <arpa/inet.h>
#include <pcap.h>
#include <linux/if_ether.h>
#include <linux/ip.h>
#include <linux/tcp.h>
#include <inttypes.h>

/* 抓取数据包的回调函数 */
void capture_packet(unsigned char *arg, const struct pcap_pkthdr *pkthdr,
                    const unsigned char *packet);

/* 
 * @description 打开指定的网络接口，并设置数据包过滤规则
 * @param iface 网络接口名，一般有线网卡是eth0，无线网卡是wlan0，在进行测试的时候使用回环地址，因此是lo
 * @param bpf, Berkeley Packet Filter， 数据包过滤规则
 * @return 返回pcap_t的指针
 * */
pcap_t *capture_live(const char *iface, const char *bpf);
