#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/time.h>
#include <arpa/inet.h>

#define PORT 12345
#define BUFFER_SIZE 1024

int main() {
    int sockfd;
    char buffer[BUFFER_SIZE];
    struct sockaddr_in server_addr;
    struct timeval start, end;

    // Creating socket
    if ((sockfd = socket(AF_INET, SOCK_DGRAM, 0)) < 0) {
        perror("Socket creation failed");
        exit(EXIT_FAILURE);
    }

    // Setting up the server address
    memset(&server_addr, 0, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(PORT);
    server_addr.sin_addr.s_addr = INADDR_ANY;

    // Getting the current time
    gettimeofday(&start, NULL);
    sprintf(buffer, "%ld.%06ld", start.tv_sec, start.tv_usec);

    // Sending the message to the server
    sendto(sockfd, buffer, strlen(buffer), 0, (const struct sockaddr *)&server_addr, sizeof(server_addr));

    // Receiving the echoed message from the server
    int n = recvfrom(sockfd, buffer, BUFFER_SIZE, 0, NULL, NULL);
    buffer[n] = '\0';

    // Getting the current time after receiving the echoed message
    gettimeofday(&end, NULL);

    // Calculating the round-trip time (RTT)
    double rtt = (end.tv_sec - start.tv_sec) * 1000.0 + (end.tv_usec - start.tv_usec) / 1000.0;
    printf("Received from server: %s\n",buffer);
    printf("Round Trip Time (RTT): %.3f ms\n", rtt);

    // Close the socket
    close(sockfd);
    return 0;
}
