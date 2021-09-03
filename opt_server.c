#include "utils.h"
#include <netinet/in.h>
#include <string.h>
#include <sys/socket.h>
#include <unistd.h>

#define PORT 9000
#define mem_clear(var) memset(&var, 0, sizeof(var))

int main() {
  // 存储收到的内容
  char buffer[128] = {0};
  const char *msg = "Msg From Server";

  struct sockaddr_in server_addr, client_addr;

  // 创建 socket
  // 这里创建 SOCK_DGRAM，即 UDP
  int socket_fd = socket(AF_INET, SOCK_DGRAM, IPPROTO_IP);
  if (socket_fd < 0) {
    printf("Create socket failed %d\n", errno);
    return -1;
  }

  mem_clear(server_addr);
  mem_clear(client_addr);

  // 配置 server 端的地址
  server_addr.sin_family = AF_INET; // IPv4 接收本机或其他机器传输
  server_addr.sin_addr.s_addr = INADDR_ANY; // 0.0.0.0 接收所有 ip
  server_addr.sin_port = htons(PORT);       // 设置端口

  // 将服务端地址和 socket 关联，用于之后的链接
  if (bind(socket_fd, (const struct sockaddr *)&server_addr,
           sizeof(server_addr)) < 0) {
    printf("Bind socket failed %d\n", errno);
    return -1;
  }

  uint client_addr_size = sizeof(client_addr);
  for (int i = 0; i < 8; ++i) {
    // 接收数据
    uint len = recvfrom(socket_fd, buffer, 128, 0,
                        (struct sockaddr *)&client_addr, &client_addr_size);
    if (len < 0) {
      printf("Recv error %d\n", errno);
      return -1;
    }
    printf("Client msg: %s\n", buffer);
    // 从消息中取出 ddl prio
    uint ddl, prio;
    sscanf(buffer, "%u %u", &ddl, &prio);

    // 计算 tos
    int t = tos(ddl, prio);
    // 使用 setsockopt 设置 tos 段
    if (setsockopt(socket_fd, IPPROTO_IP, IP_TOS, &t, sizeof(int)) < 0)
      printf("Warning: Cannot set TOS!\n");
    // 发送数据，使用wireshark抓包看信息
    sendto(socket_fd, (const char *)msg, strlen(msg), 0,
           (const struct sockaddr *)&client_addr, client_addr_size);
    printf("Server message sent.\n");
  }

  close(socket_fd);

  return 0;
}