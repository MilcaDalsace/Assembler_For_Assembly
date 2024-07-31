#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_LINE_LENGTH 100
#define MAX_MACRO_NAME 50
#define MAX_MACRO_CONTENT 500
#define MAX_MACROS 100

typedef struct {
    char name[MAX_MACRO_NAME];
    char content[MAX_MACRO_CONTENT];
} Macro;

Macro macros[MAX_MACROS];
int macroCount = 0;

void addMacro(const char *name, const char *content) {
    if (macroCount < MAX_MACROS) {
        strcpy(macros[macroCount].name, name);
        strcpy(macros[macroCount].content, content);
        macroCount++;
    } else {
        fprintf(stderr, "Error: Maximum number of macros reached.\n");
    }
}

const char* findMacro(const char *name) {
    for (int i = 0; i < macroCount; i++) {
        if (strcmp(macros[i].name, name) == 0) {
            return macros[i].content;
        }
    }
    return NULL;
}

void processFile(const char *inputFilename) {
    char outputFilename[FILENAME_MAX];
    strcpy(outputFilename, inputFilename);
    strcat(outputFilename, ".am");

    FILE *inputFile = fopen(inputFilename, "r");
    if (!inputFile) {
        perror("Error opening input file");
        return;
    }

    FILE *outputFile = fopen(outputFilename, "w");
    if (!outputFile) {
        perror("Error opening output file");
        fclose(inputFile);
        return;
    }

    char line[MAX_LINE_LENGTH];
    char macroName[MAX_MACRO_NAME];
    char macroContent[MAX_MACRO_CONTENT];
    int insideMacro = 0;

    while (fgets(line, MAX_LINE_LENGTH, inputFile)) {
        char *token = strtok(line, " \t\n");

        if (token && strcmp(token, "macro") == 0) {
            token = strtok(NULL, " \t\n");
            if (token) {
                strcpy(macroName, token);
                macroContent[0] = '\0';
                insideMacro = 1;
            } else {
                fprintf(stderr, "Error: Macro name missing.\n");
            }
        } else if (insideMacro && token && strcmp(token, "endmacro") == 0) {
            addMacro(macroName, macroContent);
            insideMacro = 0;
        } else if (insideMacro) {
            strcat(macroContent, line);
        } else {
            const char *macroContent = findMacro(token);
            if (macroContent) {
                fprintf(outputFile, "%s", macroContent);
            } else {
                fprintf(outputFile, "%s", line);
            }
        }
    }

    fclose(inputFile);
    fclose(outputFile);
}

int main(int argc, char *argv[]) {
    if (argc != 2) {
        fprintf(stderr, "Usage: %s <source-file>\n", argv[0]);
        return 1;
    }

    processFile(argv[1]);

    return 0;
}
