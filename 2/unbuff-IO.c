#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <unistd.h>

#define NUM_ENTRIES 1000000

void unbuffered_logging() {
    FILE *file = fopen("unbufflog.txt", "w");
    if (!file) {
        perror("cant open file");
        exit(1);
    }
    
    setbuf(file, NULL);
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
    unbuffered_logging();
    return 0;
}