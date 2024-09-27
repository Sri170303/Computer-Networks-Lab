#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <netinet/in.h>

#define PORT 8080

int main() {
    int sockfd, connfd;
    struct sockaddr_in servaddr, cli;
    char buffer[1024];
    int expected_seq_num = 0;
    int ack;

    // Creating socket file descriptor
    if ((sockfd = socket(AF_INET, SOCK_STREAM, 0)) == 0) {
        perror("Socket creation failed");
        exit(EXIT_FAILURE);
    }

    memset(&servaddr, 0, sizeof(servaddr));

    // Filling server information
    servaddr.sin_family = AF_INET;
    servaddr.sin_addr.s_addr = INADDR_ANY;
    servaddr.sin_port = htons(PORT);

    // Bind the socket to the server address
    if (bind(sockfd, (struct sockaddr *)&servaddr, sizeof(servaddr)) < 0) {
        perror("Bind failed");
        exit(EXIT_FAILURE);
    }

    // Listen for incoming connections
    if (listen(sockfd, 5) < 0) {
        perror("Listen failed");
        exit(EXIT_FAILURE);
    }

    // Accept the incoming connection
    int len = sizeof(cli);
    if ((connfd = accept(sockfd, (struct sockaddr *)&cli, (socklen_t *)&len)) < 0) {
        perror("Server accept failed");
        exit(EXIT_FAILURE);
    }

    while (1) {
        memset(buffer, 0, sizeof(buffer));
        int n = recv(connfd, buffer, sizeof(buffer), 0);
        if (n > 0) {
            int received_seq_num = atoi(&buffer[7]);  // Extract sequence number
            printf("Received: %s\n", buffer);

            if (received_seq_num == expected_seq_num) {
                // Send ACK for the expected packet
                ack = received_seq_num;
                expected_seq_num++;
            } else {
                // Send ACK for the last correctly received packet
                ack = expected_seq_num - 1;
            }

            printf("Sending ACK: %d\n", ack);
            send(connfd, &ack, sizeof(int), 0);
        }
    }

    close(connfd);
    close(sockfd);
    return 0;
}