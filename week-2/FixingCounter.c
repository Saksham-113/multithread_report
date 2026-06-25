#include <stdio.h>
#include <pthread.h>

#define ITERS 10000000

pthread_mutex_t lock = PTHREAD_MUTEX_INITIALIZER;
long counter = 0;

void *increment(void *arg) {
    (void)arg;         /* unused — pthread signature requires it */
    for (long i = 0; i < ITERS; i++) {
        pthread_mutex_lock(&lock);
        counter++;     /* three machine instructions in disguise */
        pthread_mutex_unlock(&lock);
    }
    return NULL;
}

int main(void) {
    pthread_t t1, t2;

    pthread_create(&t1, NULL, increment, NULL);
    pthread_create(&t2, NULL, increment, NULL);

    pthread_join(t1, NULL);
    pthread_join(t2, NULL);
    
    printf("%ld\n", counter);
    return 0;
}