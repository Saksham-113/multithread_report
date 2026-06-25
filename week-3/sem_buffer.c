#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <semaphore.h>
#include <unistd.h>

#define BUFFER_CAPACITY 8
#define NUM_PRODUCERS 3
#define NUM_CONSUMERS 3
#define ITEMS_PER_PRODUCER 20

int *buffer;
int capacity;
int in = 0;
int out = 0;

sem_t empty;
sem_t full;           
pthread_mutex_t mutex; 

long long total_produced_sum = 0;
long long total_consumed_sum = 0;
int total_produced_count = 0;
int total_consumed_count = 0;

pthread_mutex_t verify_mutex = PTHREAD_MUTEX_INITIALIZER;

void buffer_init(int cap) {
    capacity = cap;
    buffer = malloc(sizeof(int) * capacity);
    if (buffer == NULL) {
        perror("Failed to allocate buffer");
        exit(1);
    }
    sem_init(&empty, 0, capacity); 
    sem_init(&full, 0, 0);         
    pthread_mutex_init(&mutex, NULL);
}

void buffer_put(int item) {
    sem_wait(&empty); 
    pthread_mutex_lock(&mutex);

    buffer[in] = item;
    in = (in + 1) % capacity;

    pthread_mutex_unlock(&mutex);
    sem_post(&full); 
}

int buffer_get(void) {
    sem_wait(&full); 
    pthread_mutex_lock(&mutex);

    int item = buffer[out];
    out = (out + 1) % capacity;

    pthread_mutex_unlock(&mutex);
    sem_post(&empty); 
    return item;
}

void buffer_destroy(void) {
    free(buffer);
    sem_destroy(&empty);
    sem_destroy(&full);
    pthread_mutex_destroy(&mutex);
}

void *producer(void *arg) {
    int id = *(int *)arg;
    for (int i = 0; i < ITEMS_PER_PRODUCER; i++) {
        int item = (id * 100) + i; 
        buffer_put(item);
        
        pthread_mutex_lock(&verify_mutex);
        total_produced_sum += item;
        total_produced_count++;
        pthread_mutex_unlock(&verify_mutex);
    }
    return NULL;
}

void *consumer(void *arg) {
    int total_items_to_consume = (NUM_PRODUCERS * ITEMS_PER_PRODUCER) / NUM_CONSUMERS;
    for (int i = 0; i < total_items_to_consume; i++) {
        int item = buffer_get();
        
        pthread_mutex_lock(&verify_mutex);
        total_consumed_sum += item;
        total_consumed_count++;
        pthread_mutex_unlock(&verify_mutex);
    }
    return NULL;
}

int main(void) {
    buffer_init(BUFFER_CAPACITY);

    pthread_t prods[NUM_PRODUCERS];
    pthread_t cons[NUM_CONSUMERS];
    int prod_ids[NUM_PRODUCERS];
    int cons_ids[NUM_CONSUMERS];

    for (int i = 0; i < NUM_CONSUMERS; i++) {
        cons_ids[i] = i + 1;
        pthread_create(&cons[i], NULL, consumer, &cons_ids[i]);
    }

    for (int i = 0; i < NUM_PRODUCERS; i++) {
        prod_ids[i] = i + 1;
        pthread_create(&prods[i], NULL, producer, &prod_ids[i]);
    }

    for (int i = 0; i < NUM_PRODUCERS; i++) {
        pthread_join(prods[i], NULL);
    }
    for (int i = 0; i < NUM_CONSUMERS; i++) {
        pthread_join(cons[i], NULL);
    }
    printf("Total Items Produced: %d Total Items Consumed: %d\n", total_produced_count, total_consumed_count);
    printf("Sum of Produced Jobs: %lld Sum of Consumed Jobs: %lld\n", total_produced_sum, total_consumed_sum);

    if (total_produced_count == total_consumed_count && total_produced_sum == total_consumed_sum) {
        printf("SUCCESS\n");
    } else {
        printf("FAIL\n");
    }

    buffer_destroy();
    return 0;
}