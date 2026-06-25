#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>

int balance = 10000;     

#define ATTEMPTS_PER_CUSTOMER 100000
#define WITHDRAWAL_AMOUNT     1
pthread_mutex_t lock = PTHREAD_MUTEX_INITIALIZER;

void *customer(void *arg) {
    const char *name = (const char *)arg;
    int successful = 0;
    int rejected   = 0;

    for (int i = 0; i < ATTEMPTS_PER_CUSTOMER; i++) {
        pthread_mutex_lock(&lock);
        if (balance >= WITHDRAWAL_AMOUNT) {
            balance -= WITHDRAWAL_AMOUNT;
            successful++;
        } else {
            rejected++;
        }
        pthread_mutex_unlock(&lock);
    }

    printf("Customer %s: %d successful, %d rejected\n",
           name, successful, rejected);
    return NULL;
}

int main(void) {
    pthread_t alice, bob;

    pthread_create(&alice, NULL, customer, (void *)"Alice");
    pthread_create(&bob,   NULL, customer, (void *)"Bob");

    pthread_join(alice, NULL);
    pthread_join(bob,   NULL);

    printf("Final balance: %d\n", balance);

    if (balance < 0) {
        printf("NEGATIVE\n");
    } else if (balance > 0) {
        printf("POSITIVE\n");
    } else {
        printf("0\n");
    }
    return 0;
}
