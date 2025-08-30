#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <signal.h>
#include <errno.h>
#include <fcntl.h>
#include <sys/stat.h>

#define PORT 8082
#define BUFFER_SIZE 1024
#define MAX_FILENAME 256

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

// Vulnerable function with integer overflow
void vulnerable_file_server(int client_socket) {
    char buffer[BUFFER_SIZE];
    char filename[MAX_FILENAME];
    char response[BUFFER_SIZE * 2];
    int bytes_received;
    int file_size;
    int total_bytes_to_send;
    int bytes_sent;
    int fd;
    
    printf("=== New client connected ===\n");
    printf("Buffer address: %p\n", buffer);
    printf("filename address: %p\n", filename);
    
    // Receive filename from client
    bytes_received = recv(client_socket, buffer, sizeof(buffer) - 1, 0);
    
    if (bytes_received < 0) {
        perror("Error receiving filename");
        return;
    }
    
    buffer[bytes_received] = '\0';
    
    // Extract filename (simple parsing)
    if (sscanf(buffer, "GET %s", filename) != 1) {
        strcpy(filename, buffer);  // Use entire buffer as filename
    }
    
    printf("Client requested file: %s\n", filename);
    
    // Open the requested file
    fd = open(filename, O_RDONLY);
    if (fd == -1) {
        perror("Error opening file");
        char error_msg[] = "File not found or permission denied";
        send(client_socket, error_msg, strlen(error_msg), 0);
        return;
    }
    
    // Get file size
    struct stat file_stat;
    if (fstat(fd, &file_stat) == -1) {
        perror("Error getting file size");
        close(fd);
        return;
    }
    
    file_size = file_stat.st_size;
    printf("File size: %d bytes\n", file_size);
    
    // VULNERABILITY: Integer overflow in size calculation
    // This can cause integer wraparound
    total_bytes_to_send = file_size + 100;  // Add header size
    
    printf("Total bytes to send: %d\n", total_bytes_to_send);
    
    // VULNERABILITY: Integer overflow in buffer allocation
    // If file_size is very large, this can overflow
    int buffer_size = file_size + 200;
    printf("Calculated buffer size: %d\n", buffer_size);
    
    // VULNERABILITY: Integer overflow in loop condition
    // This can cause infinite loops or buffer overflows
    int chunks_to_send = (file_size / 100) + 1;
    printf("Chunks to send: %d\n", chunks_to_send);
    
    // Send file header
    snprintf(response, sizeof(response), "HTTP/1.1 200 OK\r\nContent-Length: %d\r\n\r\n", file_size);
    send(client_socket, response, strlen(response), 0);
    
    // Send file content in chunks
    bytes_sent = 0;
    while (bytes_sent < file_size) {
        int bytes_to_read = (file_size - bytes_sent > 100) ? 100 : (file_size - bytes_sent);
        
        // VULNERABILITY: Integer overflow in read operation
        // If bytes_sent overflows, this can read beyond file bounds
        int bytes_read = read(fd, buffer, bytes_to_read);
        
        if (bytes_read <= 0) {
            break;
        }
        
        // VULNERABILITY: Integer overflow in send operation
        // If bytes_sent overflows, this can send wrong data
        int bytes_sent_this_time = send(client_socket, buffer, bytes_read, 0);
        
        if (bytes_sent_this_time <= 0) {
            break;
        }
        
        bytes_sent += bytes_sent_this_time;
        
        // VULNERABILITY: Integer overflow in progress calculation
        // This can cause infinite loops
        int progress = (bytes_sent * 100) / file_size;
        printf("Progress: %d%% (%d/%d bytes)\n", progress, bytes_sent, file_size);
    }
    
    close(fd);
    
    printf("File sent successfully\n");
    printf("=== Client disconnected ===\n\n");
}

// Function to demonstrate integer overflow
void demonstrate_integer_overflow() {
    printf("=== Integer Overflow Demonstration ===\n");
    
    // Example 1: Unsigned integer overflow
    unsigned int max_uint = 0xFFFFFFFF;
    printf("Max unsigned int: %u (0x%x)\n", max_uint, max_uint);
    printf("Max + 1: %u (0x%x) - OVERFLOW!\n", max_uint + 1, max_uint + 1);
    
    // Example 2: Signed integer overflow
    int max_int = 0x7FFFFFFF;
    printf("Max signed int: %d (0x%x)\n", max_int, max_int);
    printf("Max + 1: %d (0x%x) - OVERFLOW!\n", max_int + 1, max_int + 1);
    
    // Example 3: Size calculation overflow
    int file_size = 0x7FFFFFFF;
    int header_size = 200;
    int total_size = file_size + header_size;
    printf("File size: %d\n", file_size);
    printf("Header size: %d\n", header_size);
    printf("Total size: %d - OVERFLOW!\n", total_size);
    
    printf("\n=== Attack Examples ===\n");
    printf("1. Send very large file size to cause integer overflow\n");
    printf("2. Use overflow to bypass size checks\n");
    printf("3. Cause infinite loops with overflowed counters\n");
    printf("4. Allocate wrong buffer sizes due to overflow\n");
    printf("\n");
}

int main() {
    int client_socket;
    struct sockaddr_in server_addr, client_addr;
    socklen_t client_len = sizeof(client_addr);
    
    // Set up signal handler for graceful shutdown
    signal(SIGINT, signal_handler);
    signal(SIGTERM, signal_handler);
    
    printf("=== Vulnerable File Server ===\n");
    printf("Educational Purpose: Demonstrating Integer Overflow Vulnerabilities\n");
    printf("WARNING: This server is intentionally vulnerable!\n\n");
    
    // Demonstrate integer overflow
    demonstrate_integer_overflow();
    
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
    printf("Vulnerability: Integer overflow in size calculations\n");
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
        vulnerable_file_server(client_socket);
        
        // Close client connection
        close(client_socket);
    }
    
    return 0;
}