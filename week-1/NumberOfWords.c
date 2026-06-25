#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <pthread.h>

void* countWords(void* args);

#define NUM_THREADS 100
int word_count = 0;

typedef struct {
    char *data;
    int len;
} Chunk;

int main() { 
    char *filename = "book.md";
    FILE* file = fopen(filename, "r");

    fseek(file, 0, SEEK_END);
    long fsize = ftell(file);
    rewind(file);

    char *buf = malloc(fsize + 1);
    fread(buf, 1, fsize, file);
    buf[fsize] = '\0';
    fclose(file);

    pthread_t threads[NUM_THREADS];
    Chunk chunks[NUM_THREADS];
    long chunk_len = fsize / NUM_THREADS;

    for (int i = 0; i < NUM_THREADS; i++) {
        chunks[i].data = buf + i * chunk_len;
        chunks[i].len  = (i == NUM_THREADS - 1) ? fsize - i * chunk_len : chunk_len;
        pthread_create(&threads[i], NULL, countWords, &chunks[i]);
    }

    for (int i = 0; i < NUM_THREADS; i++)
        pthread_join(threads[i], NULL);

    printf("Total words in the book: %d\n", word_count);
    free(buf);
    return 0;
}

void* countWords(void* args) {
    Chunk *chunk = (Chunk *) args;
    int in_word = 0;

    for (int i = 0; i < chunk->len; i++) {
        char c = chunk->data[i];
        if (isspace(c)) {
            in_word = 0;
        } 
        else if (!in_word) {
            in_word = 1;
            word_count++;
        }
    }
    if (in_word) word_count++;
    return NULL;
}