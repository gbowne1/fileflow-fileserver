#include "udp.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>

void udp_listen(int port) {
    int sockfd;
    char buffer[1024];
    struct sockaddr_in server_addr, client_addr;
    socklen_t len;

    if ((sockfd = socket(AF_INET, SOCK_DGRAM, 0)) < 0) {
        perror("UDP socket creation failed");
        return;
    }

    memset(&server_addr, 0, sizeof(server_addr));
    memset(&client_addr, 0, sizeof(client_addr));

    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = INADDR_ANY;
    server_addr.sin_port = htons(port);

    if (bind(sockfd, (const struct sockaddr *)&server_addr, sizeof(server_addr)) < 0) {
        perror("UDP bind failed");
        close(sockfd);
        return;
    }

    printf("UDP server listening on port %d\n", port);

    while (1) {
        len = sizeof(client_addr);
        int n = recvfrom(sockfd, buffer, sizeof(buffer) - 1, 0,
                         (struct sockaddr *)&client_addr, &len);
        if (n > 0) {
            buffer[n] = '\0';
            if (strncmp(buffer, "LIST", 4) == 0) {
                const char *msg = "Available files:\n - foo.txt\n - bar.zip\n";
                sendto(sockfd, msg, strlen(msg), 0,
                       (const struct sockaddr *)&client_addr, len);
            }
        }
    }
}
