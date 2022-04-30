#include <string.h>
#include <stdbool.h>
#include <sys/mman.h>
#include <malloc.h>
#include "stdio.h"
#include "wzip.h"

pthread_mutex_t m;

int main(int argc, char *argv[]) {
    FILE *input_file = NULL;
    char *src;

    // NO FILES TO PARSE
    if (argc == 1) {
        printf("wzip: file1 [file2 ...]\n");
        return 1;
    }

    ret_val *master_returns = malloc(sizeof(ret_val));

    // GO THROUGH EVERY FILE
    for (int fileNum = 1; fileNum < argc; ++fileNum) {
        input_file = fopen(argv[fileNum], "r");
        if (fstat(fileno(input_file), &fileStat)) {
            printf("Bad file read");
        }
        src = mmap(0, fileStat.st_size, PROT_READ, MAP_PRIVATE, fileno(input_file), 0);

        // IF FILE SIZE IS GREATER THAN 4K CREATE THREADS
        if (fileStat.st_size > 4096) {
            pthread_t threads[3];
            arg_t arg[3];
            char *strings[3];

            size_t first = strlen(src) / 3;
            size_t second = 2 * strlen(src) / 3;

            strings[0] = malloc(strlen(src) / 3 + 1);
            strings[1] = malloc(strlen(src) / 3 + 1);
            strings[2] = malloc(strlen(src) / 3 + 1);

            strncpy(strings[0], src, first);
            strncpy(strings[1], &src[first], second - first);
            strncpy(strings[2], &src[second], strlen(src) - second);

            init_arg(&arg[0], strings[0]);
            init_arg(&arg[1], strings[1]);
            init_arg(&arg[2], strings[2]);

            pthread_create(&threads[0], NULL, &worker, &arg[0]);
            pthread_create(&threads[1], NULL, &worker, &arg[1]);
            pthread_create(&threads[2], NULL, &worker, &arg[2]);

            pthread_join(threads[0], NULL);
            pthread_join(threads[1], NULL);
            pthread_join(threads[2], NULL);

            if (fileNum > 1) {
                combine_returns(master_returns, arg[0].ret_val);
            }
            else {
                memmove(master_returns, arg[0].ret_val, sizeof(ret_val));
            }
            combine_returns(master_returns, arg[1].ret_val);
            combine_returns(master_returns, arg[2].ret_val);
        }
            // FILE SIZE IS LESS THAN 4K
        else {
            arg_t smallArg = {};
            init_arg(&smallArg, src);
            parse(&smallArg);
            if (fileNum > 1) {
                combine_returns(master_returns, smallArg.ret_val);
            }
            else {
                memmove(master_returns, smallArg.ret_val, sizeof(ret_val));
            }
        }
        fclose(input_file);
    }

    write_and_free(master_returns);

//    if (charCount > 0) {
//        res_pair *pair = malloc(sizeof(res_pair));
//        pair->numCharacters = charCount;
//        pair->character = lastChar;
//        writePair(*pair);
//        free(pair);
//    }

    return 0;
}

/**
 * Initialize the thread input and output buffers
 * @param arg
 * @param inputString
 */
void init_arg(arg_t *arg, char *inputString) {
    arg->arg_val = malloc(sizeof(arg_t));
    arg->ret_val = malloc(sizeof(arg_t));
    arg->arg_val->src = inputString;
    arg->ret_val->resultPairs = malloc(sizeof(res_pair));
    arg->ret_val->numPairs = 0;
}

void *worker(arg_t *arg) {
    pthread_mutex_lock(&m);
    parse(arg);
    pthread_mutex_unlock(&m);
    return NULL;
}


void* parse(arg_t *args) {
    char currChar;
    int charCount = 1;
    char lastChar = args->arg_val->src[0];

    args->ret_val->firstCharacter = args->arg_val->src[0];

    for (int i = 1; i < strlen(args->arg_val->src); ++i) {
        if ((i % 10000) == 0) {
            printf("Made it to %d\n", i);
        }
        currChar = args->arg_val->src[i];
        if (currChar != lastChar) {
            // not the same make pair
            args->ret_val->numPairs++;
            args->ret_val->resultPairs = realloc(args->ret_val->resultPairs, args->ret_val->numPairs * sizeof(res_pair));
            args->ret_val->resultPairs[args->ret_val->numPairs - 1].character = lastChar;
            args->ret_val->resultPairs[args->ret_val->numPairs - 1].numCharacters = charCount;
            charCount = 1;
            lastChar = currChar;
        } else {
            charCount++;
        }
    }

    args->ret_val->numPairs++;
    args->ret_val->resultPairs = realloc(args->ret_val->resultPairs, args->ret_val->numPairs * sizeof(res_pair));
    args->ret_val->resultPairs[args->ret_val->numPairs - 1].character = lastChar;
    args->ret_val->resultPairs[args->ret_val->numPairs - 1].numCharacters = charCount;
    args->ret_val->lastCharacter = lastChar;

    return NULL;
}

void combine_returns(ret_val *first, ret_val *second) {

    // CHARACTERS ARE THE SAME, COMPACT THE ARGS
    if (first->lastCharacter == second->firstCharacter) {
        first->numPairs = first->numPairs + second->numPairs - 1;
        first->resultPairs = realloc(first->resultPairs, first->numPairs * sizeof(res_pair));
        first->resultPairs[first->numPairs - 1].numCharacters += second->resultPairs[0].numCharacters;

        for (int i = 1; i < second->numPairs; ++i) {
            memmove(&first[i + first->numPairs - 1], &second->resultPairs[i], sizeof(res_pair));
        }
    }
        // NOT THE SAME, JUST COMPOUND
    else {
        first->numPairs = first->numPairs + second->numPairs;
        first->resultPairs = realloc(first->resultPairs, first->numPairs * sizeof(res_pair));

        for (int i = 0; i < second->numPairs; ++i) {
            memmove(&first[i + first->numPairs], &second->resultPairs[i], sizeof(res_pair));
        }
    }

    for (int i = 0; i < second->numPairs; ++i) {
        free(&second->resultPairs[i]);
    }
}

void writePair(res_pair pair) {
    fwrite(&pair.numCharacters, 4, 1, stdout);
    fwrite(&pair.character, 1, 1, stdout);
}

void write_and_free(ret_val *values) {
    for (int i = 0; i < values->numPairs; ++i) {
        writePair(values->resultPairs[i]);
    }
    free(values->resultPairs);
}
