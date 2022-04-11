#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <stdbool.h>
#include "stdio.h"
#include "wzip.h"


int main(int argc, char *argv[]) {
    int fileNum = 1;
    FILE *input_file = NULL;
    char concatString[9999];

    if (argc == 1) {
        printf("wzip: file1 [file2 ...]\n");
        return 1;
    }

    while (fileNum < argc) {
        char* tempstring;
        size_t lineBufSize;
        int numChars = 1;
        input_file = freopen(argv[fileNum], "r", stdin);
        stdin = input_file;
        while (numChars > 0){
            numChars = getline(&tempstring, &lineBufSize, stdin);
            if (numChars > 0){
                strcat(concatString, tempstring);
            }
        }
        fileNum++;
    }

    compressFile(concatString);

    return 0;
}

void compressFile(char * inputLine){
    char currChar;
    int currPos = 0;
    int charCount = 0;

    currChar = inputLine[0];
    while (currPos <= strlen(inputLine)) {
        if (inputLine[currPos] == currChar) {
            charCount++;
        } else {
            writeNum(charCount);
            writeChar(currChar);
            currChar = inputLine[currPos];
            charCount = 1;
        }
        if (currPos == strlen(inputLine)) {
            break;
        }
        currPos++;
    }
}

void writeChar(char character) {
    fwrite(&character, 1, 1, stdout);
}

void writeNum(int charCount) {
    fwrite(&charCount, 4, 1, stdout);
}
