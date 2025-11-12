#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>
#include <sys/types.h>
#include <string.h>
#include <fcntl.h>

int main() {
    pid_t pid;
    int status;
    int file_desc;
    char buffer[100];
    ssize_t bytes_read, bytes_written;
    
    // Create a temporary file
    printf("Parent process (PID: %d) creating temporary file...\n", getpid());
    file_desc = open("temp_file.txt", O_CREAT | O_RDWR | O_TRUNC, 0644);
    if (file_desc < 0) {
        perror("Failed to create file");
        exit(1);
    }
    
    // Write to the temporary file
    char *data = "Hello from parent process!\n";
    bytes_written = write(file_desc, data, strlen(data));
    if (bytes_written < 0) {
        perror("Failed to write to file");
        close(file_desc);
        exit(1);
    }
    printf("Written %zd bytes to file\n", bytes_written);
    pid = fork();
    
    if (pid < 0) {
        perror("Fork failed");
        close(file_desc);
        exit(1);
    }
    else if (pid == 0) {
        printf("Child: (PID: %d) executing 'ls -l' command\n", getpid());
        
        char *args[] = {"ls", "-l", NULL};
        
        execvp("ls", args);
        
        perror("failed");
        exit(1);
    }
    else {
        printf("Parent process (PID: %d)child (PID: %d)\n", 
               getpid(), pid);
        wait(&status);
        
        if (WIFEXITED(status)) {
            printf("Child process exited: %d\n", WEXITSTATUS(status));
        }
        
        lseek(file_desc, 0, SEEK_SET); 
        bytes_read = read(file_desc, buffer, sizeof(buffer) - 1);
        if (bytes_read < 0) {
            perror("Can't read file");
        } else {
            buffer[bytes_read] = '\0'; 
            printf("Parent read from file: %s", buffer);
        }
        
        close(file_desc);
        remove("temp_file.txt"); 
        printf("Temporary file cleaned up\n");
    }
    
    return 0;
}