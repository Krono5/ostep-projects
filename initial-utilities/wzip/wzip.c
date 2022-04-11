#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include "stdio.h"
#include "wzip.h"


int main(int argc, char *argv[]) {
    char *buffer[BUFF_SIZE];
    int numLines = 0;
    size_t lineBufSize = 0;
    int fileNum = 1;
    FILE *input_file = NULL;
    char concatString[9999];

    if (argc == 1) {
        return 1;
    }

    for (int i = 0; i < BUFF_SIZE; ++i) {
        buffer[i] = NULL;
    }

    while (fileNum < argc) {
        input_file = freopen(argv[fileNum], "r", stdin);
        stdin = input_file;
        getline(&buffer[numLines], &lineBufSize, stdin);
        numLines++;
        fileNum++;
    }

    for (int i = 0; i < numLines; ++i) {
        strcat(concatString, buffer[i]);
    }
    char currChar = ' ';
    int currPos = 0;
    int charCount = 0;

    currChar = concatString[0];
    while (currPos <= strlen(concatString)) {
        if (concatString[currPos] == currChar) {
            charCount++;
        } else {
            writeNum(charCount);
            writeChar(currChar);
            currChar = concatString[currPos];
            charCount = 1;
        }
        if (currPos == strlen(concatString)) {
            break;
        }
        currPos++;
    }
    return 0;
}

void writeChar(char character) {
    fwrite(&character, 1, 1, stdout);
}

void writeNum(int charCount) {
    fwrite(&charCount, 4, 1, stdout);
}
