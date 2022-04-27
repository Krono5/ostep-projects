#ifndef WZIP_H
#define WZIP_H

#include <string.h>
#include <stdbool.h>
#include <sys/stat.h>
#include "stdio.h"
#include "pthread.h"

typedef struct __ret_pair {
    char character;
    int numCharacters;
} ret_pair;

typedef struct __arg {
    struct arg_val_t {
        char *inputString;
    } arg_val;   // information passed to the thread

    struct ret_val_t {
        int numPairs;
        char firstChar;
        char lastChar;
        ret_pair *resultPairs;
    } ret_val;   // information passed back to the main thread
} arg_t;

typedef struct __file_res{
    ret_pair *filePairs;
    char firstChar;
    char lastChar;
} file_res;

struct stat fileStat;

void writeChar(char character);

void writeNum(int charCount);

void *worker(arg_t arg);

void init_arg(arg_t arg, char *inputString);

void writePair(arg_t arg, char character, int charCount);

void parseThreaded(char *src, char *buffer);


#endif //WZIP_H
