#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <signal.h>
#include <errno.h>

#define PORT 8080
#define BUFFER_SIZE 64  // Intentionally small to demonstrate overflow

// Global socket for cleanup
int server_socket = -1;

// Signal handler for graceful shutdown
void signal_handler(int sig) {
    printf("\n=== Shutting down server ===\n");
    if (server_socket != -1) {
        close(server_socket);
    }
    exit(0);
}

// Vulnerable function that copies user input without bounds checking
void vulnerable_echo(int client_socket) {
    char buffer[BUFFER_SIZE];  // Small buffer - vulnerable to overflow
    char response[BUFFER_SIZE * 2];
    int bytes_received;
    
    printf("=== New client connected ===\n");
    printf("Buffer address: %p\n", buffer);
    printf("Buffer size: %d bytes\n", BUFFER_SIZE);
    
    // Receive data from client
    bytes_received = recv(client_socket, buffer, sizeof(buffer) * 2, 0);
    
    if (bytes_received < 0) {
        perror("Error receiving data");
        return;
    }
    
    // VULNERABILITY: No bounds checking!
    // If client sends more than BUFFER_SIZE bytes, this will overflow
    printf("Received %d bytes from client\n", bytes_received);
    printf("Raw data (hex): ");
    for (int i = 0; i < bytes_received && i < 100; i++) {
        printf("%02x ", (unsigned char)buffer[i]);
    }
    printf("\n");
    
    // VULNERABILITY: strcpy without bounds checking
    strcpy(response, "Echo: ");
    strcat(response, buffer);  // This can overflow if buffer is too large
    
    printf("Sending response: %s\n", response);
    
    // Send response back to client
    send(client_socket, response, strlen(response), 0);
    
    printf("=== Client disconnected ===\n\n");
}

int main() {
    int client_socket;
    struct sockaddr_in server_addr, client_addr;
    socklen_t client_len = sizeof(client_addr);
    
    // Set up signal handler for graceful shutdown
    signal(SIGINT, signal_handler);
    signal(SIGTERM, signal_handler);
    
    printf("=== Vulnerable Echo Server ===\n");
    printf("Educational Purpose: Demonstrating Buffer Overflow Vulnerabilities\n");
    printf("WARNING: This server is intentionally vulnerable!\n\n");
    
    // Create socket
    server_socket = socket(AF_INET, SOCK_STREAM, 0);
    if (server_socket < 0) {
        perror("Error creating socket");
        exit(1);
    }
    
    // Set socket options
    int opt = 1;
    if (setsockopt(server_socket, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt)) < 0) {
        perror("Error setting socket options");
        close(server_socket);
        exit(1);
    }
    
    // Configure server address
    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = INADDR_ANY;
    server_addr.sin_port = htons(PORT);
    
    // Bind socket
    if (bind(server_socket, (struct sockaddr*)&server_addr, sizeof(server_addr)) < 0) {
        perror("Error binding socket");
        close(server_socket);
        exit(1);
    }
    
    // Listen for connections
    if (listen(server_socket, 5) < 0) {
        perror("Error listening");
        close(server_socket);
        exit(1);
    }
    
    printf("Server listening on port %d\n", PORT);
    printf("Buffer size: %d bytes\n", BUFFER_SIZE);
    printf("Vulnerability: Buffer overflow in recv() and strcpy()\n");
    printf("Press Ctrl+C to stop server\n\n");
    
    // Main server loop
    while (1) {
        printf("Waiting for connections...\n");
        
        // Accept client connection
        client_socket = accept(server_socket, (struct sockaddr*)&client_addr, &client_len);
        if (client_socket < 0) {
            perror("Error accepting connection");
            continue;
        }
        
        printf("Client connected from %s:%d\n", 
               inet_ntoa(client_addr.sin_addr), ntohs(client_addr.sin_port));
        
        // Handle client (vulnerable function)
        vulnerable_echo(client_socket);
        
        // Close client connection
        close(client_socket);
    }
    
    return 0;
}

// # Compile with debugging symbols and disable some protections for demonstration
//gcc -g -fno-stack-protector -z execstack -o vulnerable_echo_server vulnerable_echo_server.c
//echo "Hello World" | nc localhost 8080
//python3 -c "print('A' * 100)" | nc localhost 8080