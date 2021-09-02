#include "utils.h"

u_char tos(u_int ddl, u_int prio) {
  u_int tmp = ddl + prio;
  if (tmp < 0x00000010) {
    return 0 << 5;
  } else if (tmp < 0x00000100) {
    return 1 << 5;
  } else if (tmp < 0x00001000) {
    return 2 << 5;
  } else if (tmp < 0x00010000) {
    return 3 << 5;
  } else if (tmp < 0x00100000) {
    return 4 << 5;
  } else if (tmp < 0x01000000) {
    return 5 << 5;
  } else if (tmp < 0x10000000) {
    return 6 << 5;
  } else {
    return 7 << 5;
  }
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