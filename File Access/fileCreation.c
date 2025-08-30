#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <fcntl.h>
#include <string.h>
#include <pwd.h>
#include <grp.h>
#include <errno.h>

int main() {
    const char *filename = "/tmp/os_security_demo.txt";
    const char *content = "Hello welcome to OS security course";
    int fd;
    struct stat file_stat;
    struct passwd *pwd;
    struct group *grp;
    
    printf("=== File Creation (User Mallikarjun)===\n\n");
    
    // Get current user information
    uid_t current_uid = getuid();
    gid_t current_gid = getgid();
    
    pwd = getpwuid(current_uid);
    grp = getgrgid(current_gid);
    
    printf("Current User: %s (UID: %d)\n", pwd ? pwd->pw_name : "unknown", current_uid);
    printf("Current Group: %s (GID: %d)\n", grp ? grp->gr_name : "unknown", current_gid);
    printf("\n");
    
    // Create the file
    printf("Creating file: %s\n", filename);
    fd = open(filename, O_CREAT | O_WRONLY | O_TRUNC, 0600);
    
    if (fd == -1) {
        perror("Error creating file");
        return 1;
    }
    
    // Write content to file
    ssize_t bytes_written = write(fd, content, strlen(content));
    if (bytes_written == -1) {
        perror("Error writing to file");
        close(fd);
        return 1;
    }
    
    close(fd);
    printf("File created successfully with content: \"%s\"\n", content);
    printf("Initial permissions set to: 600 (owner read/write only)\n\n");
    
    // Get file information
    if (stat(filename, &file_stat) == -1) {
        perror("Error getting file information");
        return 1;
    }
    
    // Display file permissions in different formats
    printf("=== File Information ===\n");
    printf("File: %s\n", filename);
    printf("Size: %ld bytes\n", file_stat.st_size);
    
    // Show permissions in octal format
    printf("Permissions (octal): %o\n", file_stat.st_mode & 0777);
    
    // Show permissions in symbolic format
    printf("Permissions (symbolic): ");
    printf("%c", (file_stat.st_mode & S_IRUSR) ? 'r' : '-');
    printf("%c", (file_stat.st_mode & S_IWUSR) ? 'w' : '-');
    printf("%c", (file_stat.st_mode & S_IXUSR) ? 'x' : '-');
    printf("%c", (file_stat.st_mode & S_IRGRP) ? 'r' : '-');
    printf("%c", (file_stat.st_mode & S_IWGRP) ? 'w' : '-');
    printf("%c", (file_stat.st_mode & S_IXGRP) ? 'x' : '-');
    printf("%c", (file_stat.st_mode & S_IROTH) ? 'r' : '-');
    printf("%c", (file_stat.st_mode & S_IWOTH) ? 'w' : '-');
    printf("%c", (file_stat.st_mode & S_IXOTH) ? 'x' : '-');
    printf("\n");
    
    // Show ownership
    pwd = getpwuid(file_stat.st_uid);
    grp = getgrgid(file_stat.st_gid);
    printf("Owner: %s (UID: %d)\n", pwd ? pwd->pw_name : "unknown", file_stat.st_uid);
    printf("Group: %s (GID: %d)\n", grp ? grp->gr_name : "unknown", file_stat.st_gid);
    
    printf("\n=== Next Steps ===\n");
    printf("1. Switch to User Y\n");
    printf("2. Go to /tmp/os_security_demo.txt\n");
    printf("3. Observe permission denied error\n");
    printf("4. Return to User X to modify permissions\n\n");
    
    printf("Press Enter to continue...");
    getchar();
    
    return 0;
}