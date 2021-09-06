#include "utils.h"

u_char tos(u_int ddl, u_int prio) {
  // TODO 怎么设计?

  // ddl 在 0x9A7EC800切顶 1月
  // 0x5265C00 1天
  // 0x36EE80 1h
  // 0xEA60 1min
  // 0x3E8 1s
  // 0x1F4 500ms
  // 0xC8 200ms

  u_char d, p;
  if (ddl > 0x9A7EC800) {
    d = 0;
  } else if (ddl > 0x5265C00) {
    d = 1;
  } else if (ddl > 0x36EE80) {
    d = 2;
  } else if (ddl > 0xEA60) {
    d = 3;
  } else if (ddl > 0x3E8) {
    d = 4;
  } else if (ddl > 0x1F4) {
    d = 5;
  } else if (ddl > 0xC8) {
    d = 6;
  } else {
    d = 7;
  }

  if (prio > 1000000) {
    p = 0;
  } else if (prio > 100000) {
    p = 1;
  } else if (prio > 10000) {
    p = 2;
  } else if (prio > 1000) {
    p = 3;
  } else if (prio > 100) {
    p = 4;
  } else if (prio > 10) {
    p = 5;
  } else if (prio > 1) {
    p = 6;
  } else {
    p = 7;
  }
  return max(d, p);
}

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

void set_ipv4_header(struct ip *ip_header, u_char tos, u_short len,
                     in_addr_t ip_src, in_addr_t ip_dst) {
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
  ip_header->ip_src.s_addr = ip_src;
  ip_header->ip_dst.s_addr = ip_dst;
}

void set_udp_header(struct udphdr *udp_header, u_short port_src,
                    u_short port_dst, u_short len) {
  udp_header->uh_sport = htons(port_src);
  udp_header->uh_dport = htons(port_dst);
  udp_header->uh_ulen = htons(len);
}