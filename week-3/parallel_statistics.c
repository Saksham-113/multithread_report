#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <limits.h>

#define SIZE 1000000
#define NUM_THREADS 4

int arr[SIZE];

typedef struct {
    int start;
    int end;
} worker_arg_t;

typedef struct {
    long long partial_sum;
    int partial_min;
    int partial_max;
    int elements_processed;
} worker_result_t;

void *worker(void *arg) {
    worker_arg_t *range = (worker_arg_t *)arg;
    
    worker_result_t *result = malloc(sizeof(worker_result_t));
    if (result == NULL) {
        perror("Failed to allocate memory for worker result");
        pthread_exit(NULL);
    }
    
    result->partial_sum = 0;
    result->partial_min = INT_MAX;
    result->partial_max = INT_MIN;
    result->elements_processed = range->end - range->start;
    
    for (int i = range->start; i < range->end; i++) {
        int val = arr[i];
        result->partial_sum += val;
        if (val < result->partial_min) result->partial_min = val;
        if (val > result->partial_max) result->partial_max = val;
    }
    
    pthread_exit((void *)result);
}

int main(void) {
    for (int i = 0; i < SIZE; i++) {
        arr[i] = i + 1;
    }

    pthread_t threads[NUM_THREADS];
    worker_arg_t thread_args[NUM_THREADS];
    
    int slice_size = SIZE / NUM_THREADS;

    for (int i = 0; i < NUM_THREADS; i++) {
        thread_args[i].start = i * slice_size;
        thread_args[i].end = (i == NUM_THREADS - 1) ? SIZE : (i + 1) * slice_size;
        
        if (pthread_create(&threads[i], NULL, worker, &thread_args[i]) != 0) {
            perror("Failed");
            return 1;
        }
    }

    long long global_sum = 0;
    int global_min = INT_MAX;
    int global_max = INT_MIN;
    long long total_elements = 0;

    for (int i = 0; i < NUM_THREADS; i++) {
        worker_result_t *res;
        if (pthread_join(threads[i], (void **)&res) != 0) {
            perror("Failed");
        }
        
        if (res != NULL) {
            global_sum += res->partial_sum;
            if (res->partial_min < global_min) global_min = res->partial_min;
            if (res->partial_max > global_max) global_max = res->partial_max;
            total_elements += res->elements_processed;
            
            free(res);
        }
    }

    double global_avg = (double)global_sum / total_elements;

    printf("%lld\n", total_elements);
    printf("%lld\n", global_sum);
    printf("%d\n", global_min);
    printf("%d\n", global_max);
    printf("%.2f\n", global_avg);

    return 0;
}