#include "utils.h"

#define PORT 9000

// IP 包头 20 字节，TCP 包头 20 字节，UDP 包头 8 字节
// IP + max(TCP, UDP) = 40 bytes
char buffer[40];

struct ip *ip_header = (struct ip *)buffer;
struct udphdr *udp_header = (struct udphdr *)(buffer + sizeof(struct ip));
// struct tcphdr *tcp_header = (struct tcphdr *)(buffer + sizeof(struct ip));

int main() {
  // 创建一个 raw socket
  int socket_fd = socket(AF_INET, SOCK_RAW, IPPROTO_RAW);
  // int socket_fd = socket(AF_INET, SOCK_DGRAM, IPPROTO_IP);
  if (socket_fd == -1) {
    printf("create socket failed:%d\n", errno);
    return -1;
  }

  const char *msg = "server msg";
  // char recv_buffer[64];

  // 本机地址信息
  struct sockaddr_in server_addr;
  server_addr.sin_family = AF_INET;   // 接收本机或其他机器传输
  server_addr.sin_port = htons(PORT); // 设置端口号
  server_addr.sin_addr.s_addr = htonl(INADDR_ANY); // 接收所有 ip
  bzero(&(server_addr.sin_zero), 8);
  // 清零
  memset(buffer, 0, sizeof(struct ip) + sizeof(struct udphdr));

  // struct sockaddr_in client_addr;
  // memset(&client_addr, 0, sizeof(client_addr));

  // ipv4 头交给后面循环设置 tos 时设置
  // set_ipv4_header(ip_header, 0b01000000, 29, "1.2.3.4", "1.2.3.4");
  set_udp_header(udp_header, PORT, PORT + 1, 9);
  strcpy(buffer + 28, msg);

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
  // ip_header->ip_sum = checksum((u_short *)buffer, ip_header->ip_len >>
  // 1);

  /* finally, it is very advisable to do a IP_HDRINCL call, to make sure
      that the kernel knows the header is included in the data, and doesn't
      insert its own header into the packet before our data */
  {
    int one = 1;
    const int *val = &one;
    if (setsockopt(socket_fd, IPPROTO_IP, IP_HDRINCL, val, sizeof(one)) < 0)
      printf("Warning: Cannot set HDRINCL!\n");
  }

  if (connect(socket_fd, (struct sockaddr *)&server_addr,
              sizeof(struct sockaddr)) < 0) {
    printf("Connect failed %d\n", errno);
  }

  // if (bind(socket_fd, (const struct sockaddr *)&server_addr,
  //          sizeof(server_addr)) < 0) {
  //   printf("Bind failed %d\n", errno);
  //   return -1;
  // }

  // u_int n, len;
  // len = sizeof(client_addr);

  // n = recvfrom(socket_fd, recv_buffer, 64, MSG_WAITALL,
  //              (struct sockaddr *)&client_addr, &len);
  // recv_buffer[n] = '\0';

  // printf("Client: %s\n", recv_buffer);
  // set_ipv4_header(ip_header, tos(0, 1 << (1 * 4)), 29,
  //                 server_addr.sin_addr.s_addr, client_addr.sin_addr.s_addr);
  // set_udp_header(udp_header, PORT, client_addr.sin_port, 9);
  // sendto(socket_fd, buffer, 40, 0, (const struct sockaddr *)&client_addr,
  // len);

  // printf("try sent\n");
  // return 0;

  for (int i = 0; i < 8; ++i) {
    set_ipv4_header(ip_header, tos(0, 1 << (i * 4)), 29, inet_addr("127.0.0.1"),
                    inet_addr("127.0.0.1"));

    if (sendto(socket_fd,                       // raw socket
               buffer,                          // header buffer
               40,                              // packet len
               0,                               // routing flags
               (struct sockaddr *)&server_addr, // socket address
               sizeof(struct sockaddr))         // socket length
        < 0)                                    // if error
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