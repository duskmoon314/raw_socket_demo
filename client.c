#include "utils.h"

#define PORT 9000

// IP 包头 20 字节，TCP 包头 20 字节，UDP 包头 8 字节
// IP + max(TCP, UDP) = 40 bytes
char buffer[64];

struct ip *ip_header = (struct ip *)buffer;
struct udphdr *udp_header = (struct udphdr *)(buffer + sizeof(struct ip));

int main() {
  const char *msg = "client msg";

  struct sockaddr_in server_addr;

  // int socket_fd = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
  int socket_fd = socket(AF_INET, SOCK_RAW, IPPROTO_RAW);
  if (socket_fd < 0) {
    printf("Create socket error %d\n", errno);
  }

  memset(&server_addr, 0, sizeof(server_addr));

  server_addr.sin_family = AF_INET;
  server_addr.sin_port = htons(PORT);
  server_addr.sin_addr.s_addr = INADDR_ANY;

  memset(buffer, 0, sizeof(struct ip) + sizeof(struct udphdr));
  strcpy(buffer + 28, msg);
  set_ipv4_header(ip_header, 0, 29, inet_addr("127.0.0.1"),
                  inet_addr("127.0.0.1"));
  set_udp_header(udp_header, PORT + 1, PORT, 9);

  {
    int one = 1;
    const int *val = &one;
    if (setsockopt(socket_fd, IPPROTO_IP, IP_HDRINCL, val, sizeof(one)) < 0)
      printf("Warning: Cannot set HDRINCL!\n");
  }

  if (connect(socket_fd, (struct sockaddr *)&server_addr, sizeof(server_addr)) <
      0) {
    printf("Connect failed %d\n", errno);
    return -1;
  }

  if (sendto(socket_fd, buffer, 64, 0, (const struct sockaddr *)&server_addr,
             sizeof(server_addr)) < 0) {
    printf("send error %d\n", errno);
    return -1;
  } else
    printf("msg sent\n");

  u_int n, len;

  n = recvfrom(socket_fd, buffer, 64, MSG_WAITALL,
               (struct sockaddr *)&server_addr, &len);

  buffer[n] = '\0';
  printf("Server: %s\n", buffer);

  close(socket_fd);
  return 0;
}