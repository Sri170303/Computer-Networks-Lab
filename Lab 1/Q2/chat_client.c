#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <pthread.h>

#define PORT 12345
#define BUFFER_SIZE 1024

void *receive_messages(void *arg) {
    int sock = *(int *)arg;
    char buffer[BUFFER_SIZE];
    int bytes_read;

    while ((bytes_read = recv(sock, buffer, BUFFER_SIZE, 0)) > 0) {
        buffer[bytes_read] = '\0';

        // Assuming the message format is "sender: message"
        char *sender = strtok(buffer, ":");
        char *message = strtok(NULL, "");

        if (sender && message) {
            printf("\r%s: %s\n", sender, message);
        } else {
            printf("\r%s\n", buffer); // In case the format is not as expected
        }

        fflush(stdout);
    }

    printf("\nDisconnected from server.\n");
    close(sock);
    exit(0);
}

int main() {
    int sock = 0;
    struct sockaddr_in serv_addr;
    pthread_t thread_id;
    char buffer[BUFFER_SIZE];
    char name[50];

    // Prompt the user for their name
    printf("Enter your name: ");
    fgets(name, sizeof(name), stdin);
    name[strcspn(name, "\n")] = 0;  // Remove newline character

    // Create socket file descriptor
    if ((sock = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
        perror("Socket creation error");
        return -1;
    }

    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(PORT);

    // Convert IPv4 and IPv6 addresses from text to binary form
    if (inet_pton(AF_INET, "127.0.0.1", &serv_addr.sin_addr) <= 0) {
        perror("Invalid address / Address not supported");
        return -1;
    }

    // Connect to the server
    if (connect(sock, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0) {
        perror("Connection failed");
        return -1;
    }

    // Send the user's name to the server
    send(sock, name, strlen(name), 0);

    // Create a thread to receive messages
    pthread_create(&thread_id, NULL, receive_messages, &sock);
    pthread_detach(thread_id);

    // Main thread handles sending messages
    while (fgets(buffer, BUFFER_SIZE, stdin) != NULL) {
        char message[BUFFER_SIZE];
        snprintf(message, sizeof(message), "%s: %s", name, buffer);
        send(sock, message, strlen(message), 0);
    }

    close(sock);
    return 0;
}

