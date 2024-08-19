#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "preAssembler.h"
#include "transition.h"
#include "function.h"

Macro *macros = NULL;
int macroCount = 0;

/* יצירת מקרו חדש והוספתו לרשימת המקרוים*/
void addMacro(const char *name, const char *content)
{
    macroCount++;
    macros = realloc(macros, macroCount * sizeof(Macro));
    if (macros == NULL)
    { /*כבר נבדק?*/
        fprintf(stderr, "Error: Memory allocation failed.\n");
        exit(EXIT_FAILURE);
    }
    macros[macroCount - 1].name = strdup(name);
    /*strcpy(macros[macroCount - 1].name,name);*/
    macros[macroCount - 1].content = strdup(content);
    /*strcpy(macros[macroCount - 1].content,content);*/
    if (macros[macroCount - 1].name == NULL || macros[macroCount - 1].content == NULL)
    {
        fprintf(stderr, "Error: Memory allocation failed.\n");
        exit(EXIT_FAILURE);
    }
}

const char *findMacro(const char *name)
{
    for (int i = 0; i < macroCount; i++)
    {
        if (strcmp(macros[i].name, name) == 0)
        {
            return macros[i].content;
        }
    }
    return NULL;
}

int checkingWhetherTheFileIsCorrect(FILE *checkedFile)
{
    if (!checkedFile)
    {
        perror("Error opening the file");
        fclose(checkedFile);
        return 0;
    }
    return 1;
}

void removeFileAndExit(const char *outputFilename)
{
    if (remove(outputFilename) == 0)
    {
        exit(EXIT_FAILURE);
    }
}

void regetError(const char *errorLine, FILE *outputFile, char *outputFilename)
{
    fprintf(stderr, "%s \n", errorLine);
    if (outputFile && outputFilename)
    {
        fclose(outputFile);
        removeFileAndExit(outputFilename);
    }
}

void processFile(const char *inputFilename)
{
    char outputFilename[FILENAME_MAX];
    strcpy(outputFilename, inputFilename);
    char *dot;
    dot = strrchr(outputFilename, '.');
    if (dot && strcmp(dot, ".as") == 0)
    {
        *dot = '\0';
    }
    else
    {
        regetError("Error: file name is incorrect.", NULL, NULL);
    }
    strcat(outputFilename, ".am");

    FILE *inputFile = fopen(inputFilename, "r");
    if (!checkingWhetherTheFileIsCorrect(inputFile))
        return;

    FILE *outputFile = fopen(outputFilename, "w");
    if (!checkingWhetherTheFileIsCorrect(outputFile))
        return;

    char line[MAX_LINE_LENGTH];
    char linetocheck[MAX_LINE_LENGTH];
    char macroName[MAX_MACRO_NAME];
    char *macroContent = NULL;
    size_t macroContentSize = 0;
    int insideMacro = 0;
    /* מעבר על שורות הקובץ*/
    while (fgets(line, MAX_LINE_LENGTH, inputFile))
    {
        strcpy(linetocheck, line);
        char *token = strtok(linetocheck, " \t\n");

        /* בדיקה האם שורת ההגדרה של המקרו תקינה*/
        if (token && strcmp(token, "macr") == 0)
        {
            token = strtok(NULL, " \t\n");
            if (token)
            {
                /*  בדיקה האם שם המקרו תקין*/
                if (findOperation(token) + 1)
                {
                    regetError("Error: Macro name restricted.", outputFile, outputFilename);
                }
                else
                {
                    strcpy(macroName, token);
                    token = strtok(NULL, " \t\n");
                    /* בדיקה אם אין תווים מיותרים בשורת ההגדרה*/
                    if (token)
                    {
                        regetError("Error: Additional characters in the macro definition line.", outputFile, outputFilename);
                    }
                    else
                    {
                        /* שורת הגדרת שם מקרו תקינה*/
                        free(macroContent);
                        macroContent = NULL;
                        macroContentSize = 0;
                        insideMacro = 1;
                    }
                }
            }
            /* קרתה הגדרת מאקרו ללא שם אחריו*/
            else
            {
                regetError("Error: Macro name missing.", outputFile, outputFilename);
            }
        }
        /* בדיקה האם יש כאן סיום של הגדרת מאקרו*/
        else if (token && strcmp(token, "endmacr") == 0)
        {
            if (insideMacro == 0)
            {
                regetError("Error: End macr without undefine macr.", outputFile, outputFilename);
            }
            token = strtok(NULL, " \t\n");
            if (token)
            {
                regetError("Error: Additional characters at the end.", outputFile, outputFilename);
            }
            /* בדיקה האם הוגדר מאקרו ללא תוכן*/
            if (macroContent == NULL)
            {
                regetError("Error: Memory allocation failed.", outputFile, outputFilename);
            }
            /* הוספת מאקרו חדש*/
            addMacro(macroName, macroContent);
            insideMacro = 0;
        }
        /* שמירת תוכן המאקרו*/
        else if (insideMacro)
        {
            size_t lineLength = strlen(line);
            macroContent = realloc(macroContent, macroContentSize + lineLength + 1);
            if (macroContent == NULL)
            {
                regetError("Error: Memory allocation failed.", outputFile, outputFilename);
            }
            strcpy(macroContent + macroContentSize, line);
            macroContentSize += lineLength;
        }
        /* קריאה למאקרו והחלפתו בתוכן שלו*/
        else if (findMacro(token))
        {
            fprintf(outputFile, "%s", macroContent);
        }
        /* הוספת שורה רגילה לקובץ*/
        else
        {
            fprintf(outputFile, "%s", line);
        }
    }
    if (insideMacro)
    {
        regetError("Error: Macro without end.", outputFile, outputFilename);
    }
    free(macroContent);
    fclose(inputFile);
    fclose(outputFile);

    /* First pass*/
    firstPass(outputFilename);

    /* Second pass*/
    secondPass(outputFilename);

    /* Build output files*/
    buildOutputFiles(outputFilename);
}
