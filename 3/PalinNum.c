#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <stdbool.h>

#define NUM_THREADS 20
#define MAX_NUMBER 100000

int global_count = 0;
pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;

//Check if a number is palindrom type
bool is_palindrome(int num) {
    if (num < 0) return false;
    if (num < 10) return true; 
    int original = num;
    int reversed = 0;
    
    while (num > 0) {
        reversed = reversed * 10 + (num % 10);
        num /= 10;
    }
    return original == reversed;
}

//Struct for Thread.
typedef struct {
    int thread_id;
    int start;
    int end;
} thread_data_t;

//Palindrome checker
void* check_palindromes(void* arg) {
    thread_data_t* data = (thread_data_t*)arg;
    int local_count = 0;
    
    for (int i = data->start; i <= data->end; i++) {
        if (is_palindrome(i)) {
            local_count++;
        }
    }
    
    pthread_mutex_lock(&mutex);
    global_count += local_count;
    pthread_mutex_unlock(&mutex);
    
    pthread_exit(NULL);
}

int main() {
    pthread_t threads[NUM_THREADS];
    thread_data_t thread_data[NUM_THREADS];
    int segment_size = MAX_NUMBER / NUM_THREADS;
    
    //creating threads
    for (int i = 0; i < NUM_THREADS; i++) {
        thread_data[i].thread_id = i;
        thread_data[i].start = i * segment_size;
        
        //Last thread gets the scraps
        if (i == NUM_THREADS - 1) {
            thread_data[i].end = MAX_NUMBER;
        } else {
            thread_data[i].end = (i + 1) * segment_size - 1;
        }
        
        int rc = pthread_create(&threads[i], NULL, check_palindromes, (void*)&thread_data[i]);
        if (rc) {
            printf("ERRORRR %d\n", rc);
            exit(-1);
        }
    }
    
    //Thread completion wait time
    for (int i = 0; i < NUM_THREADS; i++) {
        pthread_join(threads[i], NULL);
    }
    
    //Destroy mutex
    pthread_mutex_destroy(&mutex);
    printf("The Synchronize total number of palindromic number from 0 to 100,000 is  %d\n", global_count);  
    return 0;
}