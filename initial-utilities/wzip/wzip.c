#include <string.h>
#include <stdbool.h>
#include <sys/mman.h>
#include <malloc.h>
#include "stdio.h"
#include "wzip.h"


int main(int argc, char *argv[]) {
    FILE *input_file = NULL;
    char *src;

    // NO FILES TO PARSE
    if (argc == 1) {
        printf("wzip: file1 [file2 ...]\n");
        return 1;
    }

    ret_val master_returns = {};

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

            pthread_create(&threads[0], NULL, worker, &arg[0]);
            pthread_create(&threads[1], NULL, worker, &arg[1]);
            pthread_create(&threads[2], NULL, worker, &arg[2]);

            pthread_join(threads[0], NULL);
            pthread_join(threads[1], NULL);
            pthread_join(threads[2], NULL);

        }
            // FILE SIZE IS LESS THAN 4K
        else {
            arg_t smallArg = {};
            init_arg(&smallArg, src);
            parseThreaded(&smallArg);
            if (fileNum > 1) {
                combine_returns(&master_returns, &smallArg.ret_val);
            }
            else {
                master_returns = smallArg.ret_val;
            }
//            parse(src, &isFirstChar, &charCount, &lastChar, &firstChar);
        }
        fclose(input_file);
    }

    write_and_free(&master_returns);

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
    arg->arg_val.src = inputString;
    arg->ret_val.resultPairs = malloc(sizeof(res_pair));
    arg->ret_val.numPairs = 0;
}

void *worker(arg_t arg) {
    parseThreaded(&arg);
    return NULL;
}

void parse(char *src, bool *isFirstChar, int *charCount, char *lastChar, char *firstCharacter) {
    char currChar;

    // GET THE FIRST CHAR OF THE FILE
    int charPos = 0;
    currChar = src[charPos];
    *firstCharacter = currChar;

    do {
        // IF THIS IS THE FIRST CHAR EVER READ SKIP COMPARISON
        if (*isFirstChar) {
            *isFirstChar = false;
            *lastChar = currChar;
            charPos++;
            currChar = src[charPos];
        }
            // NOT THE SAME CHAR, PRINT CURRENT COUNT AND MOVE ON
        else if (currChar != *lastChar) {
            res_pair *pair = malloc(sizeof(res_pair));
            pair->character = *lastChar;
            pair->numCharacters = *charCount;
            writePair(*pair);
            free(pair);
            *charCount = 1;
            *lastChar = currChar;
            charPos++;
            currChar = src[charPos];
        }
            // SAME CHAR INCREMENT
        else {
            *charCount = *charCount + 1;
            *lastChar = currChar;
            charPos++;
            currChar = src[charPos];
        }
    } while (charPos < strlen(src));
}


void parseThreaded(arg_t *args) {
    char currChar;
    int charCount = 1;
    char lastChar = args->arg_val.src[0];

    args->ret_val.firstCharacter = args->arg_val.src[0];

    for (int i = 1; i < strlen(args->arg_val.src); ++i) {
        currChar = args->arg_val.src[i];
        if (currChar != lastChar) {
            // not the same make pair
            args->ret_val.numPairs++;
            args->ret_val.resultPairs = realloc(args->ret_val.resultPairs, args->ret_val.numPairs * sizeof(res_pair));
            args->ret_val.resultPairs[args->ret_val.numPairs - 1].character = lastChar;
            args->ret_val.resultPairs[args->ret_val.numPairs - 1].numCharacters = charCount;
            charCount = 1;
            lastChar = currChar;
        } else {
            charCount++;
        }
    }

    args->ret_val.numPairs++;
    args->ret_val.resultPairs = realloc(args->ret_val.resultPairs, args->ret_val.numPairs * sizeof(res_pair));
    args->ret_val.resultPairs[args->ret_val.numPairs - 1].character = lastChar;
    args->ret_val.resultPairs[args->ret_val.numPairs - 1].numCharacters = charCount;
    args->ret_val.lastCharacter = lastChar;
}

ret_val *combine_returns(ret_val *first, ret_val *second) {
    ret_val *returnPairSet;

    // CHARACTERS ARE THE SAME, COMPACT THE ARGS
    if (first->lastCharacter == second->firstCharacter) {
        returnPairSet = malloc((first->numPairs + second->numPairs - 1) * sizeof(res_pair));
        first->resultPairs[first->numPairs].numCharacters += second->resultPairs[0].numCharacters;

        for (int i = 0; i < first->numPairs; ++i) {
            memmove(&returnPairSet[i], &first->resultPairs[i], sizeof(res_pair));
        }
        for (int i = 1; i < second->numPairs; ++i) {
            memmove(&returnPairSet[i + first->numPairs - 1], &second->resultPairs[i], sizeof(res_pair));
        }
    }
        // NOT THE SAME, JUST COMPOUND
    else {
        returnPairSet = malloc((first->numPairs + second->numPairs) * sizeof(res_pair));
        for (int i = 0; i < first->numPairs; ++i) {
            memmove(&returnPairSet[i], &first->resultPairs[i], sizeof(res_pair));
        }
        for (int i = 0; i < second->numPairs; ++i) {
            memmove(&returnPairSet[i + first->numPairs], &second->resultPairs[i], sizeof(res_pair));
        }
    }

    for (int i = 0; i < first->numPairs; ++i) {
        free(&first->resultPairs[i]);
    }
    for (int i = 0; i < second->numPairs; ++i) {
        free(&second->resultPairs[i]);
    }

    return returnPairSet;
}

void writePair(res_pair pair) {
    fwrite(&pair.numCharacters, 4, 1, stdout);
    fwrite(&pair.character, 1, 1, stdout);
}

void write_and_free(ret_val *values) {
    for (int i = 0; i < values->numPairs; ++i) {
        res_pair *pair = &values->resultPairs[i];
        writePair(*pair);
        free(pair);
    }
}
