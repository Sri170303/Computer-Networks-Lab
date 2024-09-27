#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <netinet/in.h>

#define PORT 8080
#define MAX_SEQ 5  // Maximum sequence number (window size)
#define TIMEOUT 2  // Timeout in seconds

int main() {
    int sockfd;
    struct sockaddr_in servaddr;
    char buffer[1024];
    int base = 0, next_seq_num = 0;
    int ack;

    // Creating socket file descriptor
    if ((sockfd = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
        perror("Socket creation failed");
        exit(EXIT_FAILURE);
    }

    memset(&servaddr, 0, sizeof(servaddr));

    // Filling server information
    servaddr.sin_family = AF_INET;
    servaddr.sin_port = htons(PORT);
    servaddr.sin_addr.s_addr = INADDR_ANY;

    // Connect to the server
    if (connect(sockfd, (struct sockaddr *)&servaddr, sizeof(servaddr)) != 0) {
        perror("Connection to the server failed");
        exit(EXIT_FAILURE);
    }

    while (1) {
        if (next_seq_num < base + MAX_SEQ) {
            sprintf(buffer, "Packet %d", next_seq_num);
            printf("Sending: %s\n", buffer);
            send(sockfd, buffer, strlen(buffer), 0);
            next_seq_num++;
        }

        // Simulate waiting for an ACK
        int n = recv(sockfd, &ack, sizeof(int), 0);
        if (n > 0) {
            printf("Received ACK: %d\n", ack);
            base = ack + 1;

            if (base == next_seq_num) {
                // All packets acknowledged, stop timer
            } else {
                // Start timer for the unacknowledged packets
            }
        }

        // Simulate timeout (for simplicity, using sleep here)
        sleep(TIMEOUT);
        if (next_seq_num < base + MAX_SEQ) {
            // Timeout occurred, retransmit all packets from base
            printf("Timeout occurred. Resending packets from %d to %d\n", base, next_seq_num - 1);
            for (int i = base; i < next_seq_num; i++) {
                sprintf(buffer, "Packet %d", i);
                printf("Resending: %s\n", buffer);
                send(sockfd, buffer, strlen(buffer), 0);
            }
        }
    }

    close(sockfd);
    return 0;
}