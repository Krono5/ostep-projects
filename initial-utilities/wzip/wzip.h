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
    struct arg_val_t {
        char *src;
        bool *isFirstChar;
        int *charCount;
        char *lastChar;
        char *firstChar;
    } arg_val;   // information passed to the thread

    struct ret_val_t {
        char firstCharacter;
        char lastCharacter;
        res_pair *resultPairs;
    } ret_val;   // information passed back to the main thread
} arg_t;

typedef struct __file_res {
    res_pair *filePairs;
    char firstChar;
    char lastChar;
} file_res;

struct stat fileStat;

void *worker(arg_t arg);

void init_arg(arg_t arg, char *inputString, bool *isFirstChar);

void parse(char *src, bool *isFirstChar, int *charCount, char *lastChar, char *firstCharacter);

void writePair(res_pair pair);

#endif //WZIP_H
