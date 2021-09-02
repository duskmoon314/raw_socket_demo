#include <arpa/inet.h>
#include <errno.h>
#include <netinet/in.h>
#include <netinet/ip.h>
#include <netinet/tcp.h>
#include <netinet/udp.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>

/*
include:

arpa/inet.h:    inet_addr 函数
errno.h:        errno 用于输出错误信息
netinet/in.h:   协议相关宏, sockaddr_in 结构
netinet/ip.h:   ip header 结构
netinet/tcp.h:  tcp header 结构
netinet/udp.h:  udp header 结构
stdio.h:        printf
string.h:       memset
sys/socket.h:   socket, sendto 函数和相关宏
sys/types.h:    类型
unistd.h:       系统接口 close
*/

#define PORT 9000

// UDP 校验用伪 header
struct UdpPseudoHdr {
  // 32 bits
  struct in_addr src_addr;
  struct in_addr dst_addr;
  // 8 bits
  u_char zeroes;
  u_char protocol;
  // 16 bits
  u_short length;
  struct udphdr udp_header;
} UdpPseudoHdr;

// TCP 校验用伪 header
struct TcpPseudoHdr {
  // 32 bits
  struct in_addr src_addr;
  struct in_addr dst_addr;
  // 8 bits
  u_char zeroes;
  u_char protocol;
  // 16 bits
  u_short length;
  struct tcphdr tcp_header;
} TcpPseudoHdr;

// IP 包头 20 字节，TCP 包头 20 字节，UDP 包头 8 字节
// IP + max(TCP, UDP) = 40 bytes
char buffer[40];

struct ip *ip_header = (struct ip *)buffer;
struct udphdr *udp_header = (struct udphdr *)(buffer + sizeof(struct ip));
struct tcphdr *tcp_header = (struct tcphdr *)(buffer + sizeof(struct ip));

// 计算校验和
u_short checksum(u_short *buf, u_short n) {
  u_long sum = 0;
  for (; n > 0; n--) {
    sum += *buf++;
  }
  sum = (sum >> 16) + (sum & 0xffff);
  sum += (sum >> 16);
  return ~sum;
}

/**
  设置 ipv4 包头
  @param ip_header 指针指向 ip 包头
  @param tos 设置 tos 字段
  @param len 包长度
  @param ip_src 源地址
  @param ip_dst 目的地址
  @return void 无返回
 */
void set_ipv4_header(struct ip *ip_header, u_char tos, u_short len,
                     const char *ip_src, const char *ip_dst) {
  // 设置 IP 头
  ip_header->ip_v = 4;           // version = 4, IPv4
  ip_header->ip_hl = 5;          // ip header length = 5 * 32 bits = 20 bytes
  ip_header->ip_tos = tos;       // tos, need to be tested
  ip_header->ip_len = len;       // packet length
  ip_header->ip_id = 0;          // 设置 identification, 似乎 demo 中无用
  ip_header->ip_off = 0;         // 设置 fragment offset
  ip_header->ip_ttl = 255;       // 设置 ttl 为最大
  ip_header->ip_p = IPPROTO_UDP; // 设置 protocol 为 udp
  ip_header->ip_sum = 0;         // 稍后算 checksum
  ip_header->ip_src.s_addr = inet_addr(ip_src);
  ip_header->ip_dst.s_addr = inet_addr(ip_dst);
}

/**
  设置 udp 包头
  @param udp_header 指针指向 udp 包头
  @param port_src 源端口
  @param port_dst 目的端口
  @param len 包长度
  @return void 无返回
 */
void set_udp_header(struct udphdr *udp_header, u_short port_src,
                    u_short port_dst, u_short len) {
  udp_header->uh_sport = htons(port_src);
  udp_header->uh_dport = htons(port_dst);
  udp_header->uh_ulen = htons(len);
}

int main() {
  // 创建一个 raw socket
  int socket_fd = socket(AF_INET, SOCK_RAW, IPPROTO_UDP);
  if (socket_fd == -1) {
    printf("create socket failed:%d\n", errno);
    return -1;
  }

  // 本机地址信息
  struct sockaddr_in src_addr;
  src_addr.sin_family = AF_INET;   // 接收本机或其他机器传输
  src_addr.sin_port = htons(PORT); // 设置端口号
  src_addr.sin_addr.s_addr = htonl(INADDR_ANY); // 接收所有 ip
  bzero(&(src_addr.sin_zero), 8);
  // 清零
  memset(buffer, 0, sizeof(struct ip) + sizeof(struct udphdr));

  // ipv4 头交给后面循环设置 tos 时设置
  // set_ipv4_header(ip_header, 0b01000000, 29, "1.2.3.4", "1.2.3.4");
  set_udp_header(udp_header, PORT, PORT + 1, 9);
  buffer[28] = 49;

  // // 设置 udp 的 checksum, 0 不会报错
  // memset(&UdpPseudoHdr, 0, 12 + sizeof(struct udphdr));
  // UdpPseudoHdr.src_addr.s_addr = inet_addr("1.2.3.4");
  // UdpPseudoHdr.dst_addr.s_addr = inet_addr("1.2.3.4");
  // UdpPseudoHdr.protocol = IPPROTO_UDP;
  // UdpPseudoHdr.length = htons(sizeof(struct udphdr));
  // bcopy((char *)udp_header, (char *)&UdpPseudoHdr.udp_header,
  //       sizeof(struct udphdr));
  // udp_header->uh_sum =
  //     checksum((u_short *)&UdpPseudoHdr, 13 + sizeof(struct udphdr));

  // // 计算 checksum
  // ip_header->ip_sum = checksum((u_short *)buffer, ip_header->ip_len >> 1);

  /* finally, it is very advisable to do a IP_HDRINCL call, to make sure
    that the kernel knows the header is included in the data, and doesn't
    insert its own header into the packet before our data */
  {
    int one = 1;
    const int *val = &one;
    if (setsockopt(socket_fd, IPPROTO_IP, IP_HDRINCL, val, sizeof(one)) < 0)
      printf("Warning: Cannot set HDRINCL!\n");
  }

  if (connect(socket_fd, (struct sockaddr *)&src_addr,
              sizeof(struct sockaddr)) < 0) {
    printf("Connect failed %d\n", errno);
  }

  for (int i = 0; i < 8; ++i) {
    set_ipv4_header(ip_header, i << 5, 29, "1.2.3.4", "1.2.3.4");

    if (sendto(socket_fd,                    // raw socket
               buffer,                       // header buffer
               40,                           // packet len
               0,                            // routing flags
               (struct sockaddr *)&src_addr, // socket address
               sizeof(struct sockaddr))      // socket length
        < 0)                                 // if error
    {
      printf("error %d\n", errno);
      return -1;
    } else {
      printf("%d", i);
    }
  }
  close(socket_fd);
  return 0;
}