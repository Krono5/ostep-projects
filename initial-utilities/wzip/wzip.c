#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <stdbool.h>
#include "stdio.h"
#include "wzip.h"


int main(int argc, char *argv[]) {
    FILE *input_file = NULL;
    char currChar;
    bool firstChar = true;
    char lastChar;
    int charCount = 1;

    // NO FILES TO PARSE
    if (argc == 1) {
        printf("wzip: file1 [file2 ...]\n");
        return 1;
    }

    // GO THROUGH EVERY FILE
    for (int fileNum = 1; fileNum < argc; ++fileNum) {
        input_file = freopen(argv[fileNum], "r", stdin);

        // GET THE FIRST CHAR OF THE FILE
        currChar = (char) fgetc(input_file);
        do {
            // IF THIS IS THE FIRST CHAR EVER READ
            if (firstChar){
                firstChar = false;
//                charCount++;
            }
            // NOT THE SAME CHAR, PRINT CURRENT COUNT AND MOVE ON
            else if(currChar != lastChar){
                writeNum(charCount);
                writeChar(lastChar);
                charCount = 1;
            }
            // SAME CHAR INCREMENT
            else{
                charCount++;
            }
            lastChar = currChar;
            currChar = (char) fgetc(input_file);
        } while (currChar != EOF);

        fclose(input_file);
    }

    if (charCount > 0){
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