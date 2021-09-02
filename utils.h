#pragma once

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
};

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
};

u_char tos(u_int ddl, u_int prio);

u_short checksum(u_short *buf, u_short n);

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
                     in_addr_t ip_src, in_addr_t ip_dst);

/**
  设置 udp 包头
  @param udp_header 指针指向 udp 包头
  @param port_src 源端口
  @param port_dst 目的端口
  @param len 包长度
  @return void 无返回
 */
void set_udp_header(struct udphdr *udp_header, u_short port_src,
                    u_short port_dst, u_short len);