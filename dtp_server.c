#include "dtp_config.h"
#include "utils.h"
#include <netinet/in.h>
#include <string.h>
#include <sys/socket.h>
#include <unistd.h>

#define PORT 9000
#define mem_clear(var) memset(&var, 0, sizeof(var))

int main() {

  dtp_config *traces;
  int trace_num;
  traces = parse_dtp_config("test_trace.txt", &trace_num);

  // 存储收到的内容
  char buffer[128] = {0};
  //   const char *msg = "Msg From Server";
  char msg[1350] = {0};
  strcpy(msg, "Server Msg");

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
  for (int i = 0; i < 9; ++i) {
    // 接收数据
    uint len = recvfrom(socket_fd, buffer, 128, 0,
                        (struct sockaddr *)&client_addr, &client_addr_size);
    if (len < 0) {
      printf("Recv error %d\n", errno);
      return -1;
    }
    printf("\n================\nClient msg: %s\n", buffer);

    // 计算 tos
    int t = tos(traces[i].deadline, traces[i].priority) << 5;
    printf("trace %d %d %d %d\n", traces[i].deadline, traces[i].priority,
           traces[i].block_size, t >> 5);
    // 使用 setsockopt 设置 tos 段
    if (setsockopt(socket_fd, IPPROTO_IP, IP_TOS, &t, sizeof(int)) < 0)
      printf("Warning: Cannot set TOS!\n");
    // 发送数据，使用wireshark抓包看信息，每1350个byte发送一次
    sprintf(msg + 10, "%d %d %d %d###", traces[i].deadline, traces[i].priority,
            traces[i].block_size, t >> 5);
    for (int blk = traces[i].block_size; blk > 0; blk -= 1350) {
      sendto(socket_fd, (const char *)msg,
             (blk < 0 ? blk + 1350 : min(blk, 1350)), 0,
             (const struct sockaddr *)&client_addr, client_addr_size);
      printf("Server message sent.\n");
    }
  }

  // 关闭 socket 退出
  close(socket_fd);

  return 0;
}