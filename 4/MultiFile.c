#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <string.h>
#include <sys/stat.h>
#include <time.h>
#include <unistd.h>
#include <errno.h>

//Struct for passing data efficency
typedef struct {
    char* input_filename;
    char* output_filename;
    off_t* current_offset;
    pthread_mutex_t* offset_mutex;
    int thread_id;
    long file_size;
} thread_data_t;

struct timespec start_time, end_time;

//Func to get file size
long get_file_size(const char* filename) {
    struct stat st;
    if (stat(filename, &st) == 0) {
        return st.st_size;
    }
    return -1;
}

//Getting CPU core count
int get_cpu_cores() {
#ifdef _SC_NPROCESSORS_ONLN
    return sysconf(_SC_NPROCESSORS_ONLN);
#else
    // Alternative methods for different systems
    FILE* cpuinfo = fopen("/proc/cpuinfo", "r");
    if (cpuinfo) {
        int cores = 0;
        char line[256];
        while (fgets(line, sizeof(line), cpuinfo)) {
            if (strncmp(line, "processor", 9) == 0) {
                cores++;
            }
        }
        fclose(cpuinfo);
        return cores > 0 ? cores : 1;
    }
    return 1; 
#endif
}

void* merge_file(void* arg) {
    thread_data_t* data = (thread_data_t*)arg;
    
    printf("Thread is %d: Processing %s (size: %ld bytes)\n", 
           data->thread_id, data->input_filename, data->file_size);
    
    //GET THAT INPUT FILE
    FILE* input_file = fopen(data->input_filename, "rb");
    if (!input_file) {
        fprintf(stderr, "Thread %d: Errorrrrrr %s: %s\n", 
                data->thread_id, data->input_filename, strerror(errno));
        pthread_exit(NULL);
    }
    
    //Open output file for writing and reading
    FILE* output_file = fopen(data->output_filename, "r+b");
    if (!output_file) {
        fprintf(stderr, "Thread %d: Can't open output file %s: %s\n", 
                data->thread_id, data->output_filename, strerror(errno));
        fclose(input_file);
        pthread_exit(NULL);
    }
    
    char buffer[4096];
    size_t bytes_read;
    
    while ((bytes_read = fread(buffer, 1, sizeof(buffer), input_file)) > 0) {
        pthread_mutex_lock(data->offset_mutex);
        
        if (fseek(output_file, *data->current_offset, SEEK_SET) != 0) {
            fprintf(stderr, "Thread %d: Error finding output fileee\n", data->thread_id);
            pthread_mutex_unlock(data->offset_mutex);
            break;
        }
        
        //Writing data to output file
        size_t bytes_written = fwrite(buffer, 1, bytes_read, output_file);
        if (bytes_written != bytes_read) {
            fprintf(stderr, "Thread %d: Error writing to output file %zu, expected %zu\n", 
                    data->thread_id, bytes_written, bytes_read);
            pthread_mutex_unlock(data->offset_mutex);
            break;
        }
        
        *data->current_offset += bytes_written;
        pthread_mutex_unlock(data->offset_mutex);
        fflush(output_file);
    }
    
    fclose(input_file);
    fclose(output_file);
    
    printf("Thread %d: Completed processing %s\n", data->thread_id, data->input_filename);
    pthread_exit(NULL);
}

//Measuring time differenc in miliseconds
double get_time_diff_ms() {
    return (end_time.tv_sec - start_time.tv_sec) * 1000.0 + 
           (end_time.tv_nsec - start_time.tv_nsec) / 1000000.0;
}

int main(int argc, char* argv[]) {
    if (argc < 3) {
        fprintf(stderr, "Usage: %s merged_output.txt <number_of_threads>\n", argv[0]);
        fprintf(stderr, "Example: %s output.log 2\n", argv[0]);
        fprintf(stderr, "file1.txt and file2.txt getting merged together\n");
        return 1;
    }
    
    char* output_filename = argv[1];
    int num_threads = atoi(argv[2]);
    
    // Define input files - using file1.txt and file2.txt that exist in your directory
    char* input_files[] = {"file1.txt", "file2.txt"};
    int num_input_files = 2;
    
    if (num_threads <= 0) {
        fprintf(stderr, "Error: Number of threads HAS TO BE POSITIVE\n");
        return 1;
    }
    
    if (num_input_files < num_threads) {
        printf("Note: More threads (%d) than input files (%d). Using %d threads\n", 
               num_threads, num_input_files, num_input_files);
        num_threads = num_input_files;
    }
    
    printf("Merging %d files into %s\n", num_input_files, output_filename);
    
    //System INFO
    int cpu_cores = get_cpu_cores();
    printf("System has %d CPU cores\n", cpu_cores);
    
    //Verifying input files
    for (int i = 0; i < num_input_files; i++) {
        FILE* test_file = fopen(input_files[i], "rb");
        if (!test_file) {
            fprintf(stderr, "Error: Input file %s cannot be opened: %s\n", 
                    input_files[i], strerror(errno));
            return 1;
        }
        fclose(test_file);
    }
    
    FILE* output_file = fopen(output_filename, "wb");
    if (!output_file) {
        fprintf(stderr, "Error creating output file %s: %s\n", output_filename, strerror(errno));
        return 1;
    }
    fclose(output_file);
    
    off_t current_offset = 0;
    pthread_mutex_t offset_mutex;
    if (pthread_mutex_init(&offset_mutex, NULL) != 0) {
        fprintf(stderr, "Error initializing mutex\n");
        return 1;
    }
    
    //Creating thread data and threads
    pthread_t* threads = malloc(num_threads * sizeof(pthread_t));
    thread_data_t* thread_data = malloc(num_threads * sizeof(thread_data_t));
    
    if (!threads || !thread_data) {
        fprintf(stderr, "Cant allocate memory for threads\n");
        return 1;
    }
    
    // Start performance measurement
    clock_gettime(CLOCK_MONOTONIC, &start_time);
    
    //ACC threads right here. This their creation.
    int threads_created = 0;
    for (int i = 0; i < num_threads; i++) {
        if (i < num_input_files) {
            thread_data[i].input_filename = input_files[i];
            thread_data[i].output_filename = output_filename;
            thread_data[i].current_offset = &current_offset;
            thread_data[i].offset_mutex = &offset_mutex;
            thread_data[i].thread_id = i;
            thread_data[i].file_size = get_file_size(input_files[i]);
            
            printf("Assigning file %s to thread %d\n", input_files[i], i);
            
            if (pthread_create(&threads[i], NULL, merge_file, &thread_data[i]) != 0) {
                fprintf(stderr, "Error can' create thread %d\n", i);
                continue;
            }
            
            threads_created++;
        }
    }
    
    // Wait for all threads to complete
    for (int i = 0; i < threads_created; i++) {
        pthread_join(threads[i], NULL);
    }
    clock_gettime(CLOCK_MONOTONIC, &end_time);

    pthread_mutex_destroy(&offset_mutex);
    free(threads);
    free(thread_data);
    
    double execution_time = get_time_diff_ms();
    printf("\nMerge is DONE!\n");
    printf("Total execution time: %.2f ms\n", execution_time);
    
    long output_size = get_file_size(output_filename);
    printf("Final output file size: %ld bytes\n", output_size);
    
    // Calculate and display input file sizes for verification
    long total_input_size = 0;
    for (int i = 0; i < num_input_files; i++) {
        long size = get_file_size(input_files[i]);
        total_input_size += size;
        printf("Input file %s: %ld bytes\n", input_files[i], size);
    }
    printf("Total input size: %ld bytes\n", total_input_size);
    
    if (output_size == total_input_size) {
        printf("Merge successful!\n");
    } else {
        printf("Output file size (%ld) doesn't match total input size (%ld)\n", 
               output_size, total_input_size);
    }
    
    return 0;
}