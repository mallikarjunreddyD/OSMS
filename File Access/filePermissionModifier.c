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

void display_permissions(const char *filename) {
    struct stat file_stat;
    struct passwd *pwd;
    struct group *grp;
    
    if (stat(filename, &file_stat) == -1) {
        perror("Error getting file information");
        return;
    }
    
    printf("Current permissions (octal): %o\n", file_stat.st_mode & 0777);
    
    printf("Current permissions (symbolic): ");
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
    
    pwd = getpwuid(file_stat.st_uid);
    grp = getgrgid(file_stat.st_gid);
    printf("Owner: %s (UID: %d)\n", pwd ? pwd->pw_name : "unknown", file_stat.st_uid);
    printf("Group: %s (GID: %d)\n", grp ? grp->gr_name : "unknown", file_stat.st_gid);
}

int main() {
    const char *filename = "/tmp/os_security_demo1.txt";
    struct stat file_stat;
    
    printf("=== OS Security Course - Permission Modifier (User X) ===\n\n");
    
    // Check if file exists
    if (stat(filename, &file_stat) == -1) {
        perror("Error: File does not exist");
        printf("Please run user_x_file_creator first!\n");
        return 1;
    }
    
    printf("File: %s\n", filename);
    printf("\n=== Current File Permissions ===\n");
    display_permissions(filename);
    
    printf("\n=== Permission Change Demonstration ===\n");
    printf("We will demonstrate both octal and symbolic permission changes:\n");
    printf("1. 600 → 644 (add read permission for group and others)\n");
    printf("2. 644 → 666 (add write permission for group and others)\n\n");
    
    // Step 1: Change from 600 to 644 using octal notation
    printf("--- Step 1: Octal Notation (600 → 644) ---\n");
    printf("Command: chmod(filename, 0644)\n");
    printf("This adds read permission for group and others\n");
    
    if (chmod(filename, 0644) == -1) {
        perror("Error changing permissions to 644");
        return 1;
    }
    
    printf("✅ Permissions changed successfully!\n");
    printf("New permissions:\n");
    display_permissions(filename);
    
    printf("\nPress Enter to continue to next step...");
    getchar();
    
    // Step 2: Change from 644 to 666 using symbolic notation
    printf("\n--- Step 2: Symbolic Notation (644 → 666) ---\n");
    printf("Command: chmod(filename, S_IRUSR | S_IWUSR | S_IRGRP | S_IWGRP | S_IROTH | S_IWOTH)\n");
    printf("This adds write permission for group and others\n");
    
    mode_t new_mode = S_IRUSR | S_IWUSR | S_IRGRP | S_IWGRP | S_IROTH | S_IWOTH;
    
    if (chmod(filename, new_mode) == -1) {
        perror("Error changing permissions to 666");
        return 1;
    }
    
    printf("✅ Permissions changed successfully!\n");
    printf("New permissions:\n");
    display_permissions(filename);
    
    printf("\n=== Permission Change Summary ===\n");
    printf("600 (rw-------): Owner read/write, no access for others\n");
    printf("644 (rw-r--r--): Owner read/write, group/others read only\n");
    printf("666 (rw-rw-rw-): Everyone read/write\n");
    
    printf("\n=== Alternative Symbolic Commands ===\n");
    printf("You could also use these equivalent commands:\n");
    printf("chmod o+r filename    # Add read permission for others\n");
    printf("chmod g+r filename    # Add read permission for group\n");
    printf("chmod a+r filename    # Add read permission for all\n");
    printf("chmod o+w filename    # Add write permission for others\n");
    printf("chmod g+w filename    # Add write permission for group\n");
    printf("chmod a+w filename    # Add write permission for all\n");
    
    printf("\n=== Next Steps ===\n");
    printf("1. Switch to User Y\n");
    printf("2. Try: cat /tmp/os_security_demo.txt\n");
    printf("3. You should now be able to read the file!\n");
    printf("4. Try: echo 'test' >> /tmp/os_security_demo.txt\n");
    printf("5. You should now be able to write to the file too!\n\n");
    
    printf("Press Enter to continue...");
    getchar();
    
    return 0;
}