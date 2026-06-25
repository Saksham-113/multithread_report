#include <stdio.h>
#include <pthread.h>

#define ITERS 1e5

long counter = 0;

void *increment(void *arg) {
    (void)arg;
    for (long i = 0; i < ITERS; i++) {
        int tmp = counter;
        for (int j = 0; j < 1e4; j++);
        tmp = tmp + 1;
        counter = tmp;
    }
    return NULL;
}

int main(void) {
    pthread_t t1, t2;

    pthread_create(&t1, NULL, increment, NULL);
    pthread_create(&t2, NULL, increment, NULL);

    pthread_join(t1, NULL);
    pthread_join(t2, NULL);

    printf("%ld \n", counter);
    return 0;
}
