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

typedef struct __arg {
    arg_val arg_val;
    ret_val ret_val;
} arg_t;


struct stat fileStat;

void *worker(arg_t arg);

void init_arg(arg_t arg, char *inputString, bool *isFirstChar);

void parse(char *src, bool *isFirstChar, int *charCount, char *lastChar, char *firstCharacter);

void writePair(res_pair pair);

void parseThreaded(arg_t *args);

ret_val* combine_returns(struct ret_val_t *first, struct ret_val_t *second);

void write_all_pairs(res_pair *result_pairs, int num_pairs);

#endif //WZIP_H
