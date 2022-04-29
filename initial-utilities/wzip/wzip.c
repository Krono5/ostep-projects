#include <string.h>
#include <stdbool.h>
#include <sys/mman.h>
#include <malloc.h>
#include "stdio.h"
#include "wzip.h"


int main(int argc, char *argv[]) {
    FILE *input_file = NULL;
    bool isFirstChar = true;
    char lastChar;
    char *src;
    int charCount = 1;
    char firstChar;

    // NO FILES TO PARSE
    if (argc == 1) {
        printf("wzip: file1 [file2 ...]\n");
        return 1;
    }

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
            strings[0] = malloc(sizeof(src) / 3 + 1);
            strings[1] = malloc(sizeof(src) / 3 + 1);
            strings[2] = malloc(sizeof(src) / 3 + 1);

            strncpy(strings[0], src, strlen(src) / 3);
            strncpy(strings[1], &src[strlen(src) / 3], strlen(src) / 3);
            strncpy(strings[2], &src[2 * (strlen(src) / 3)], strlen(src) / 3 + 1);

            init_arg(arg[0], strings[0], &isFirstChar);
            init_arg(arg[1], strings[1], &isFirstChar);
            init_arg(arg[2], strings[2], &isFirstChar);

            pthread_create(&threads[0], NULL, worker, &arg[0]);
            pthread_create(&threads[1], NULL, worker, &arg[1]);
            pthread_create(&threads[2], NULL, worker, &arg[2]);

            pthread_join(threads[0], NULL);
            pthread_join(threads[1], NULL);
            pthread_join(threads[2], NULL);
        }
            // FILE SIZE IS LESS THAN 4K
        else {
            parse(src, &isFirstChar, &charCount, &lastChar, &firstChar);
        }
        fclose(input_file);
    }

    if (charCount > 0) {
        res_pair *pair = malloc(sizeof(res_pair));
        pair->numCharacters = charCount;
        pair->character = lastChar;
        writePair(*pair);
        free(pair);
    }

    return 0;
}

/**
 * Initialize the thread input and output buffers
 * @param arg
 * @param inputString
 */
void init_arg(arg_t arg, char *inputString, bool *isFirstChar) {
    arg.arg_val.src = inputString;
    *arg.arg_val.isFirstChar = isFirstChar;
    arg.ret_val.resultPairs = NULL;
}

void *worker(arg_t arg) {
    parse(arg.arg_val.src, arg.arg_val.isFirstChar, arg.arg_val.charCount, arg.arg_val.lastChar, arg.arg_val.firstChar);
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

    return;
}

void writePair(res_pair pair) {
    fwrite(&pair.numCharacters, 4, 1, stdout);
    fwrite(&pair.character, 1, 1, stdout);
}