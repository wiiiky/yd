#include "detect.h"

int main(int argc, char *argv[])
{
    /*
     * tcp[13]表示tcp首部的第十三个字节，网络字节序，对应的是标志字段
     *
     * tcp[13]==0x12表示捕获带有SYN和ACK标志的数据包，这种数据包是对SYN的响应
     * tcp[13]==0x10表示带有ACK的数据包，这种数据包是一般的TCP响应
     * 如果主机受到SYN链接，那么主机会返回SYN+ACK的数据包，也就是第一种，
     * 如果远程主机是正常链接，那么接下去还有一个ACK的确认包，
     * 如果远程主机是攻击性的，那么就不会有一个ACK的确认包。
     */
    pcap_t *pcap = capture_live("lo", "tcp[13]==0x12 or tcp[13]==0x10");
    if (pcap == NULL) {
        fprintf(stderr, "Permission???\n");
        exit(EXIT_FAILURE);
    }
    pcap_loop(pcap, -1, capture_packet, NULL);

    pcap_close(pcap);

    return 0;
}
