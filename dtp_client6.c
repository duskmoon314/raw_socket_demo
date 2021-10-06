// Client side implementation of UDP client-server model
#include "dtp_config.h"
#include "utils.h"
#include <arpa/inet.h>
#include <netinet/in.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>

#define PORT 9000
#define MAXLINE 1350

// Driver code
int main() {
  dtp_config *traces;
  int trace_num;
  traces = parse_dtp_config("trace.txt", &trace_num);

  int sockfd;
  char buffer[MAXLINE] = {0};
  char msg[MAXLINE] = {0};
  struct sockaddr_in6 servaddr;

  // Creating socket file descriptor
  if ((sockfd = socket(AF_INET6, SOCK_DGRAM, 0)) < 0) {
    perror("socket creation failed");
    exit(EXIT_FAILURE);
  }

  memset(&servaddr, 0, sizeof(servaddr));

  // Filling server information
  servaddr.sin6_family = AF_INET6;
  servaddr.sin6_port = htons(PORT);
  inet_pton(AF_INET6, "::1", &servaddr.sin6_addr);

  int correct = 0;

  for (int i = 0; i < 100; ++i) {
    u_int n = 0, len;

    sprintf(msg, "%d, %d", 1 << (i * 4), 1 << (i * 4));

    sendto(sockfd, (const char *)msg, strlen(msg), 0,
           (const struct sockaddr *)&servaddr, sizeof(servaddr));
    printf("\n================\nClient message sent.\n");
    // while (n >= 0) {
    n = recvfrom(sockfd, (char *)buffer, MAXLINE, 0,
                 (struct sockaddr *)&servaddr, &len);
    buffer[64] = '\0';
    printf("Server : %d %s\n", n, buffer);
    int ddl, prio, blk, t;
    sscanf(buffer, "Server Msg%d %d %d %d###", &ddl, &prio, &blk, &t);
    if (ddl == traces[i].deadline && prio == traces[i].priority &&
        blk == traces[i].block_size && t == tos(ddl, prio)) {
      correct += 1;
      printf("correct\n");
    }
    blk -= 1350;
    int cnt = 1;
    while (blk > 0) {
      recvfrom(sockfd, (char *)buffer, MAXLINE, 0, (struct sockaddr *)&servaddr,
               &len);
      printf("dump%d ", cnt);
      blk -= 1350;
      cnt += 1;
    }

    // sleep(10);
    // }
  }
  printf("\n\nCorrect : %d\n", correct);
  close(sockfd);
  return 0;
}
