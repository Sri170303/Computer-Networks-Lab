#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>

#define PORT 12345
#define BUFFER_SIZE 1024

// Function to calculate the factorial of a number
int reverseNum(int N) {
  
      // Function to store the reversed number
    int rev = 0;
    while (N > 0) {

        // Extract the last digit
        int dig = N % 10;

        // Append the digit to the reversed number
        rev = rev * 10 + dig;

        // Remove the last digit
        N /= 10;
    }
    return rev;
}

int isPalindrome(int N) {
  
    // Negative numbers are not palindromes
    if (N < 0)
        return 0;
    return N == reverseNum(N);
}


int main() {
    int sockfd;
    char buffer[BUFFER_SIZE];
    struct sockaddr_in server_addr, client_addr;
    socklen_t addr_len = sizeof(client_addr);

    // Creating socket
    if ((sockfd = socket(AF_INET, SOCK_DGRAM, 0)) < 0) {
        perror("Socket creation failed");
        exit(EXIT_FAILURE);
    }

    // Setting up the server address
    memset(&server_addr, 0, sizeof(server_addr));
    server_addr.sin_family = AF_INET; 
    server_addr.sin_addr.s_addr = INADDR_ANY;
    server_addr.sin_port = htons(PORT);

    // Binding the socket to the server address
    if (bind(sockfd, (const struct sockaddr *)&server_addr, sizeof(server_addr)) < 0) {
        perror("Bind failed");
        close(sockfd);
        exit(EXIT_FAILURE);
    }

    printf("Palindrome Checking Server started, waiting for client connections...\n");

    while (1) {
        // Receiving the number from the client
        int n = recvfrom(sockfd, buffer, BUFFER_SIZE, 0, (struct sockaddr *)&client_addr, &addr_len);
        buffer[n] = '\0';

        int number = atoi(buffer);
        bool result = isPalindrome(number);

        // Convert result to string and send it back to the client
        sprintf(buffer, "%llu", result);
        sendto(sockfd, buffer, strlen(buffer), 0, (struct sockaddr *)&client_addr, addr_len);

        printf("Is %d Palindrome: %llu\n", number, result);
    }

    // Close the socket
    close(sockfd);
    return 0;
}
