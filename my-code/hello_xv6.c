#include "kernel/types.h"
#include "user/user.h"

#define N 23

int main(int a, char *b[]) {
    printf("Hello, xv6! This is Saksham Bhadani.\n");
    printf("Number of arguments: %d\n", a - 1);
    
    for(int i = 1; i < a; i++){
        printf("arg %d: %s\n", i, b[i]);
    }
    
    printf("xv6 knows %d system calls.\n", N);
    exit(0);
}