#include <malloc.h>
#include "wzip.h"
#include "sys/mman.h"


int main(int argc, char *argv[]) {
    FILE *input_file = NULL;
    char *src;
    bool firstRead = true;

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
            parseThreaded(src);
            if (firstRead) {

                firstRead = false;
            } else {

            }
        }
            // IF FILE IS LESS THAN 4K USE THIS MAIN THREAD
        else {
            if (firstRead) {

                firstRead = false;
            } else {

            }

        }
        fclose(input_file);
    }

    if (charCount > 0) {
        writeNum(charCount);
        writeChar(lastChar);
    }

    return 0;
}

void writeChar(char character) {
    fwrite(&character, 1, 1, stdout);
}

void writeNum(int charCount) {
    fwrite(&charCount, 4, 1, stdout);
}

/**
 * Thread worker to get the number of characters in order in the section provided
 * @param arg struct of input and output from thread
 * @return
 */
void *worker(arg_t arg) {
    char currChar;
    char lastChar = '\0';
    int charCount = 0;
    int charIndex = 0;


    // GET THE FIRST CHAR OF THE FILE
    currChar = arg.arg_val.inputString[0];
    arg.ret_val.firstChar = currChar;
    do {
        // SAME CHAR INCREMENT OR FIRST CHAR
        if ((currChar == lastChar) || lastChar == '\0') {
            charCount++;
            lastChar = currChar;
            charIndex++;
            currChar = arg.arg_val.inputString[charIndex];
        }
            // NOT THE SAME CHAR, PRINT CURRENT COUNT AND MOVE ON
        else if (currChar != lastChar) {
            writePair(arg, lastChar, charCount);
            charCount = 1;
            lastChar = currChar;
            charIndex++;
            currChar = arg.arg_val.inputString[charIndex];
        }
    } while (charIndex <= strlen(arg.arg_val.inputString));
    arg.ret_val.lastChar = lastChar;
    return NULL;
}

/**
 * Initialize the thread input and output buffers
 * @param arg
 * @param inputString
 */
void init_arg(arg_t arg, char *inputString) {
    arg.arg_val.inputString = inputString;
    arg.ret_val.numPairs = 0;
    arg.ret_val.resultPairs = NULL;
    arg.ret_val.firstChar = '\0';
    arg.ret_val.lastChar = '\0';
}

/**
 * Write the pair of character to character count to the arg buffer
 * @param arg
 * @param character
 * @param charCount
 */
void writePair(arg_t arg, char character, int charCount) {
    ret_pair newPair;
    newPair.character = character;
    newPair.numCharacters = charCount;
    if (arg.ret_val.resultPairs == NULL) {
        arg.ret_val.resultPairs = malloc(sizeof(ret_pair));
    } else {
        arg.ret_val.resultPairs = realloc(arg.ret_val.resultPairs, sizeof(arg.ret_val.resultPairs) + sizeof(ret_pair));
    }
    arg.ret_val.resultPairs[arg.ret_val.numPairs] = newPair;
    arg.ret_val.numPairs++;
}

/**
 * Input is greater than 4K. Create threads
 * @param src
 */
void parseThreaded(char *src) {
    pthread_t threads[3];
    arg_t arg[3];
    char *strings[3];

    // INITIALIZE THE STRINGS
    for (int i = 0; i < 3; ++i) {
        strings[i] = malloc((fileStat.st_size / 3) + 1);
        memset(strings[i], '\0', sizeof(strlen(strings[i])));
    }

    strncpy(strings[0], src, fileStat.st_size / 3);
    strncpy(strings[1], src + fileStat.st_size, fileStat.st_size / 3);
    strncpy(strings[2], src + 2 * (fileStat.st_size), fileStat.st_size / 3);

    init_arg(arg[0], strings[0]);
    init_arg(arg[1], strings[1]);
    init_arg(arg[2], strings[2]);

    pthread_create(&threads[0], NULL, worker, &arg[0]);
    pthread_create(&threads[1], NULL, worker, &arg[1]);
    pthread_create(&threads[2], NULL, worker, &arg[2]);

    pthread_join(threads[0], NULL);
    pthread_join(threads[1], NULL);
    pthread_join(threads[2], NULL);

    // IF THE LAST CHAR OF THE FIRST THREAD MATCHES THE FIRST OF THE 2ND JOIN

//    if (arg[0].ret_val.resultPairs[arg[0].ret_val.numPairs].character ==
//        arg[1].ret_val.resultPairs[0].character) {
//        arg[0].ret_val.resultPairs[arg[0].ret_val.numPairs].numCharacters += arg[1].ret_val.resultPairs[0].numCharacters;
//        arg[1].ret_val.resultPairs[0].character = '\0';
//    }
    // IF THE LAST CHAR OF THE 2ND THREAD MATCHES THE FIRST OF THE 3RD JOIN
    if (arg[1].ret_val.resultPairs[arg[1].ret_val.numPairs].character ==
        arg[2].ret_val.resultPairs[0].character) {
        arg[1].ret_val.resultPairs[arg[1].ret_val.numPairs].numCharacters += arg[2].ret_val.resultPairs[0].numCharacters;
        arg[2].ret_val.resultPairs[0].character = '\0';
    }

    int numPairs = arg[0].ret_val.numPairs + arg[1].ret_val.numPairs + arg[2].ret_val.numPairs;
    buffer = realloc(buffer, sizeof(buffer) + (numPairs * (sizeof (char ) + sizeof (int ))));

    for (int i = 0; i < 3; ++i) {
        for (int j = 0; j < arg[i].ret_val.numPairs; ++j) {
            if(arg[i].ret_val.resultPairs[j].character != '\0'){

            }
        }
    }
}