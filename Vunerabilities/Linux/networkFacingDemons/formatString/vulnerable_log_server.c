#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <signal.h>
#include <errno.h>
#include <time.h>

#define PORT 8081
#define BUFFER_SIZE 256

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

// Function to get current timestamp
void get_timestamp(char *timestamp, size_t size) {
    time_t now = time(0);
    struct tm *tm_info = localtime(&now);
    strftime(timestamp, size, "%Y-%m-%d %H:%M:%S", tm_info);
}

// Vulnerable function with format string vulnerability
void vulnerable_log(int client_socket) {
    char buffer[BUFFER_SIZE];
    char log_message[BUFFER_SIZE * 2];
    char timestamp[32];
    int bytes_received;
    
    printf("=== New client connected ===\n");
    printf("Buffer address: %p\n", buffer);
    printf("log_message address: %p\n", log_message);
    
    // Receive data from client
    bytes_received = recv(client_socket, buffer, sizeof(buffer) - 1, 0);
    
    if (bytes_received < 0) {
        perror("Error receiving data");
        return;
    }
    
    // Null-terminate the received data
    buffer[bytes_received] = '\0';
    
    printf("Received %d bytes from client\n", bytes_received);
    printf("Client message: %s\n", buffer);
    
    // Get current timestamp
    get_timestamp(timestamp, sizeof(timestamp));
    
    // VULNERABILITY: Format string vulnerability!
    // The client input is used directly in printf without proper formatting
    printf("=== VULNERABLE LOGGING ===\n");
    printf("Timestamp: %s\n", timestamp);
    printf("Client IP: %s\n", "127.0.0.1");  // Simplified for demo
    
    // VULNERABILITY: Direct use of user input in printf
    // This allows format string attacks!
    printf("Log entry: ");
    printf(buffer);  // DANGEROUS! User controls the format string
    printf("\n");
    
    // Also vulnerable in sprintf
    sprintf(log_message, "[%s] %s", timestamp, buffer);
    printf("Formatted log: %s\n", log_message);
    
    // Send response back to client
    char response[] = "Message logged successfully";
    send(client_socket, response, strlen(response), 0);
    
    printf("=== Client disconnected ===\n\n");
}

// Function to demonstrate memory reading
void demonstrate_memory_read() {
    printf("=== Memory Layout Demonstration ===\n");
    
    int local_var = 0x12345678;
    char *secret = "SECRET_DATA";
    
    printf("local_var address: %p, value: 0x%x\n", &local_var, local_var);
    printf("secret address: %p, value: %s\n", secret, secret);
    printf("Stack pointer: %p\n", __builtin_frame_address(0));
    
    printf("\n=== Format String Attack Examples ===\n");
    printf("Use these format specifiers to exploit:\n");
    printf("%%p - Print pointer values from stack\n");
    printf("%%x - Print hex values from stack\n");
    printf("%%s - Print strings from stack (can cause crashes)\n");
    printf("%%n - Write to memory (dangerous!)\n");
    printf("%%08x - Print 8-digit hex with leading zeros\n");
    printf("\n");
}

int main() {
    int client_socket;
    struct sockaddr_in server_addr, client_addr;
    socklen_t client_len = sizeof(client_addr);
    
    // Set up signal handler for graceful shutdown
    signal(SIGINT, signal_handler);
    signal(SIGTERM, signal_handler);
    
    printf("=== Vulnerable Log Server ===\n");
    printf("Educational Purpose: Demonstrating Format String Vulnerabilities\n");
    printf("WARNING: This server is intentionally vulnerable!\n\n");
    
    // Demonstrate memory layout
    demonstrate_memory_read();
    
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
    printf("Vulnerability: Format string in printf() and sprintf()\n");
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
        vulnerable_log(client_socket);
        
        // Close client connection
        close(client_socket);
    }
    
    return 0;
}

//Compile with debugging symbols and disable some protections
//gcc -g -fno-stack-protector -o vulnerable_log_server vulnerable_log_server.c

/*

# Run the server
./vulnerable_log_server

# Test with normal input
echo "Hello World" | nc localhost 8081

# Test format string attacks
echo "%p %p %p %p" | nc localhost 8081
echo "%x %x %x %x" | nc localhost 8081
echo "%s %s %s" | nc localhost 8081
echo "%s" | nc localhost 8081 

echo "AAAA%n" | nc localhost 8081


*/
