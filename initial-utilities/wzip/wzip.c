#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include "stdio.h"
#include "wzip.h"


int main(int argc, char *argv[]) {
    char *readLine;
    size_t lineBufSize = 0;
    size_t lineSize;

    if (argc != 1) {
        FILE *input_file = NULL;
        input_file = freopen(argv[1], "r", stdin);
        if (input_file == NULL || argv[2] != NULL) {
            return (EXIT_FAILURE);
        } else {
            stdin = input_file;
        }
    }

    lineSize = getline(&readLine, &lineBufSize, stdin);
    char currChar = ' ';
//    while (currChar != '\0'){
    int currPos = 0;
    int charCount = 0;
    currChar = readLine[0];
    while (currPos <= strlen(readLine)) {
        if (readLine[currPos] == currChar) {
            charCount++;
        } else {
            fwrite(&charCount, 4, 1, stdout);
            fwrite(&currChar, 1, 1, stdout);
            currChar = readLine[currPos];
            charCount = 1;
        }
        if(currPos == strlen(readLine)){
            break;
        }
        currPos++;
//        }
//        lineSize = getline(&readLine, &lineBufSize, stdin);
    }

    free(readLine);
    return 0;
}
