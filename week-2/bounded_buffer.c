#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#define NUM_PRODUCERS 3
#define NUM_CONSUMERS 2
#define ITEMS_TO_PRODUCE 100

int* buffer;
int buffer_size = 0;
int items_consumed = 0; //because multiple consumers can't rely on ITEMS_TO_PRODUCE
//for checking bugs
int items_produced = 0; 
int consumed_sum = 0;
int produced_sum = 0;
//for using buffer
int count = 0;
int in = 0;
int out = 0;

pthread_mutex_t buffer_lock = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t not_full = PTHREAD_COND_INITIALIZER;
pthread_cond_t not_empty = PTHREAD_COND_INITIALIZER;

void buffer_init(int capacity);
void buffer_put(int item);
int buffer_get(void);
void buffer_destroy(void);

void* producer(void* arg) {
    int id = (int)(intptr_t)arg;
    for (int i = id * ITEMS_TO_PRODUCE; i < (id + 1) * ITEMS_TO_PRODUCE; i++) {
        pthread_mutex_lock(&buffer_lock);
        while (count == buffer_size) {
            printf("buffer full\n");
            pthread_cond_wait(&not_full, &buffer_lock);
        }
        buffer_put(i);
        items_produced++;
        produced_sum += i;
        printf("produced %d\n", i);
        pthread_cond_broadcast(&not_empty);
        pthread_mutex_unlock(&buffer_lock);
        usleep(50000);
    }
    return NULL;
}

void* consumer(void* arg) {
    (void)arg;
    while(1) {
        pthread_mutex_lock(&buffer_lock);
        if (items_consumed >= (NUM_PRODUCERS * ITEMS_TO_PRODUCE)) {
            pthread_mutex_unlock(&buffer_lock);
            break; 
        }
        while (count == 0) {
            printf("buffer empty\n");
            pthread_cond_wait(&not_empty, &buffer_lock);
        }
        int item = buffer_get();
        items_consumed++;
        consumed_sum += item;
        if (items_consumed == NUM_PRODUCERS * ITEMS_TO_PRODUCE) pthread_cond_broadcast(&not_empty);
        printf("consumed %d\n", item);
        pthread_cond_broadcast(&not_full);
        pthread_mutex_unlock(&buffer_lock);
        usleep(120000);
    }
    return NULL;
}

int main(void) {
    buffer_init(10);
    pthread_t producers[NUM_PRODUCERS];
    pthread_t consumers[NUM_CONSUMERS];
    for (int i = 0; i < NUM_PRODUCERS; i++) {
        pthread_create(&producers[i], NULL, producer, (void *)(intptr_t)i);
    }
    for (int i = 0; i < NUM_CONSUMERS; i++) {
        pthread_create(&consumers[i], NULL, consumer, NULL);
    }
    for (int i = 0; i < NUM_PRODUCERS; i++) {
        pthread_join(producers[i], NULL);
    }
    for (int i = 0; i < NUM_CONSUMERS; i++) {
        pthread_join(consumers[i], NULL);
    }
    printf("%d %d\n", items_produced, items_consumed);
    printf("%d %d\n", produced_sum, consumed_sum);
    buffer_destroy();
    return 0;
}

void buffer_init(int capacity) {
    buffer = malloc(capacity * sizeof(int));
    buffer_size = capacity;
}

void buffer_put(int item) {
    buffer[in] = item;
    in = (in + 1) % buffer_size;
    count++;
}

int buffer_get(void) {
    int item = buffer[out];
    out = (out + 1) % buffer_size;
    count--;
    return item;
}

void buffer_destroy(void) {
    free(buffer);
}