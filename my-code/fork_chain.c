#include "kernel/types.h"
#include "user/user.h"

void f(int d, int m, int p) {
    printf("Depth %d: PID %d, Parent PID %d\n", d, getpid(), p);
    
    if (d == m - 1) {
        printf("I am the leaf! No more children.\n");
        exit(0);
    }
    
    int id = getpid();
    int r = fork();
    
    if (r == 0) {
        f(d + 1, m, id);
    } else {
        wait(0);
    }
    
    if (d == 0) {
        printf("Total processes in chain: %d\n", m);
    }
    
    exit(0);
}

int main(int a, char *b[]) {
    int n = 5;
    
    if (a == 2) {
        n = atoi(b[1]);
    }
    
    f(0, n, 2);
    exit(0);
}