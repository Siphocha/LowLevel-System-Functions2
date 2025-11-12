#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <unistd.h>

#define NUM_ENTRIES 1000000
#define BUFFER_SIZE 8192  //8KB buffer because MEMORRRRYYY

void buffered_logging() {
    FILE *file = fopen("bufflog.txt", "w");
    if (!file) {
        perror("Cant open file");
        exit(1);
    }

    char buffer[BUFFER_SIZE];
    setvbuf(file, buffer, _IOFBF, BUFFER_SIZE);
    
    time_t rawtime;
    struct tm *timeinfo;
    char timestamp[20];
    
    for (int i = 0; i < NUM_ENTRIES; i++) {
        time(&rawtime);
        timeinfo = localtime(&rawtime);
        strftime(timestamp, sizeof(timestamp), "%Y-%m-%d %H:%M:%S", timeinfo);
        
        fprintf(file, "Entry %d: %s\n", i, timestamp);
    }
    fclose(file);
}

int main() {
    buffered_logging();
    return 0;
}