#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>

#define PORT 12345
#define BUFFER_SIZE 1024
#define MAX_CLIENTS 100

typedef struct {
    struct sockaddr_in addr; // Client address
    int id; // Unique client identifier
    char name[20]; // Client name
} Client;

Client clients[MAX_CLIENTS];
int client_count = 0;

void add_client(struct sockaddr_in *client_addr) {
    clients[client_count].addr = *client_addr;
    clients[client_count].id = client_count + 1;
    snprintf(clients[client_count].name, 20, "Client_%d", clients[client_count].id);
    client_count++;
}

char *get_client_name(struct sockaddr_in *client_addr) {
    for (int i = 0; i < client_count; i++) {
        if (clients[i].addr.sin_addr.s_addr == client_addr->sin_addr.s_addr && 
            clients[i].addr.sin_port == client_addr->sin_port) {
            return clients[i].name;
        }
    }
    return NULL;
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

    printf("Server started, waiting for connections...\n");

    while (1) {
        // Receiving data from the client
        int n = recvfrom(sockfd, buffer, BUFFER_SIZE, 0, (struct sockaddr *)&client_addr, &addr_len);
        buffer[n] = '\0';

        // Check if client is new, if so, add to the client list
        char *client_name = get_client_name(&client_addr);
        if (client_name == NULL) {
            add_client(&client_addr);
            client_name = get_client_name(&client_addr);
            printf("New connection assigned name: %s\n", client_name);
        }

        // Prepare the message to send back to the client
        char message[BUFFER_SIZE + 50];
        snprintf(message, sizeof(message), "%s: %s", client_name, buffer);

        // Send the received data back to the client (echo)
        sendto(sockfd, message, strlen(message), 0, (struct sockaddr *)&client_addr, addr_len);

        // Print the client's information and the received message
        printf("Received from %s.\nCycles Taken: %s\n", client_name, buffer);
    }

    // Close the socket
    close(sockfd);
    return 0;
}

