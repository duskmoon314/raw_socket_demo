// Client side implementation of UDP client-server model
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
  int sockfd;
  char buffer[MAXLINE] = {0};
  char msg[MAXLINE] = {0};
  struct sockaddr_in servaddr;

  // Creating socket file descriptor
  if ((sockfd = socket(AF_INET, SOCK_DGRAM, 0)) < 0) {
    perror("socket creation failed");
    exit(EXIT_FAILURE);
  }

  memset(&servaddr, 0, sizeof(servaddr));

  // Filling server information
  servaddr.sin_family = AF_INET;
  servaddr.sin_port = htons(PORT);
  servaddr.sin_addr.s_addr = INADDR_ANY;

  for (int i = 0; i < 9; ++i) {
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
    blk -= 1350;
    printf("ddl %d prio %d blk - 1350: %d t %d\n", ddl, prio, blk, t);
    int cnt = 0;
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
  close(sockfd);
  return 0;
}
