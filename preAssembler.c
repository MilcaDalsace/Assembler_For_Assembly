#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_LINE_LENGTH 1024
#define MAX_MACROS 100
#define MAX_MACRO_NAME 100
#define MAX_MACRO_CONTENT 1000

typedef struct {
    char name[MAX_MACRO_NAME];
    char content[MAX_MACRO_CONTENT];
} Macro;

Macro macros[MAX_MACROS];
int macroCount = 0;

void addMacro(const char *name, const char *content) {
    strcpy(macros[macroCount].name, name);
    strcpy(macros[macroCount].content, content);
    macroCount++;
}

char* getMacroContent(const char *name) {
    for (int i = 0; i < macroCount; i++) {
        if (strcmp(macros[i].name, name) == 0) {
            return macros[i].content;
        }
    }
    return NULL;
}

void processFile(FILE *inputFile, FILE *outputFile) {
    char line[MAX_LINE_LENGTH];
    int isMacro = 0;
    char macroName[MAX_MACRO_NAME];
    char macroContent[MAX_MACRO_CONTENT] = "";

    while (fgets(line, sizeof(line), inputFile)) {
        if (strncmp(line, "macro", 5) == 0) {
            isMacro = 1;
            sscanf(line, "macro %s", macroName);
            macroContent[0] = '\0';
            continue;
        }
        
        if (isMacro) {
            if (strncmp(line, "endmacro", 8) == 0) {
                isMacro = 0;
                addMacro(macroName, macroContent);
            } else {
                strcat(macroContent, line);
            }
            continue;
        }

        // Check if the line contains a macro name and replace it with the macro content
        for (int i = 0; i < macroCount; i++) {
            if (strstr(line, macros[i].name)) {
                char *pos = strstr(line, macros[i].name);
                char newLine[MAX_LINE_LENGTH];
                strncpy(newLine, line, pos - line);
                newLine[pos - line] = '\0';
                strcat(newLine, macros[i].content);
                strcat(newLine, pos + strlen(macros[i].name));
                strcpy(line, newLine);
            }
        }

        fputs(line, outputFile);
    }
}

int main(int argc, char *argv[]) {
    if (argc != 3) {
        printf("Usage: %s <input file> <output file>\n", argv[0]);
        return 1;
    }

    FILE *inputFile = fopen(argv[1], "r");
    if (!inputFile) {
        perror("Failed to open input file");
        return 1;
    }

    FILE *outputFile = fopen(argv[2], "w");
    if (!outputFile) {
        perror("Failed to open output file");
        fclose(inputFile);
        return 1;
    }

    processFile(inputFile, outputFile);

    fclose(inputFile);
    fclose(outputFile);

    return 0;
}
