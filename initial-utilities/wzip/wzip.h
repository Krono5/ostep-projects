#ifndef WZIP_H
#define WZIP_H

#include <string.h>
#include <stdbool.h>
#include <sys/stat.h>
#include "stdio.h"
#include "pthread.h"

typedef struct __res_pair {
    char character;
    int numCharacters;
} res_pair;

typedef struct ret_val_t {
    char firstCharacter;
    char lastCharacter;
    int numPairs;
    res_pair *resultPairs;
} ret_val;   // information passed back to the main thread

typedef struct arg_val_t {
    char *src;
    int *charCount;
    char *lastChar;
    char *firstChar;
} arg_val;   // information passed to the thread

typedef struct __arg {
    arg_val* arg_val;
    ret_val* ret_val;
} arg_t;


struct stat fileStat;

void *worker(arg_t *arg);

void init_arg(arg_t *arg, char *inputString);

void writePair(res_pair pair);

void* parse(arg_t *args);

void combine_returns(ret_val *first, ret_val *second);

void write_and_free(ret_val *values);

#endif //WZIP_H
