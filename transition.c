#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <math.h>
#include <ctype.h>
#include "transition.h"
#include "function.h"

/* First pass*/
void firstPass(const char *sourceFilename)
{
    FILE *sourceFile = fopen(sourceFilename, "r");
    if (!sourceFile)
    {
        perror("Error opening input file");
        return;
    }

    char line[MAX_LINE_LENGTH];
    char lineToCheck[MAX_LINE_LENGTH];
    int uncorrect = 0;
    int countLine = 0;
    char codeBin[CODE_SEGMENT_SIZE];
    char *newSymbolName = NULL;

    while (fgets(line, MAX_LINE_LENGTH, sourceFile))
    {
        countLine++;
        strcpy(lineToCheck, line);
        char *token = strtok(lineToCheck, " \t\n");
        if (token)
        {
            newSymbolName = labelDefinition(token); /* Check if it is a new label*/
            if ((newSymbolName))
            { /* Label definition*/
                if (isNameRestricted(newSymbolName))
                { /* Check if it is a restricted name*/
                    fprintf(stderr, "Error: line %d %s  is restricted name.\n", countLine, newSymbolName);
                    uncorrect = 1;
                }
                else
                {                                                                                /* A new correct label*/
                    token = strtok(NULL, " \t\n");                                               /* Next field*/
                    fprintf(stderr, "line: %d %s naw symbol name.\n", countLine, newSymbolName); /* check*/
                }
            }
            if (token)
            {
                /* Data*/
                if (strcmp(token, ".data") == 0)
                {
                    char *remaining = strtok(NULL, "");
                    if (addData(remaining, countLine, newSymbolName) == 0)
                    {
                        uncorrect = 1;
                    }
                }
                /* String*/
                else if (strcmp(token, ".string") == 0)
                {
                    char *remaining = strtok(NULL, "");
                    if (addString(remaining, countLine, newSymbolName) == 0)
                    {
                        uncorrect = 1;
                    }
                }
                /* Extern*/
                else if (strcmp(token, ".extern") == 0)
                {
                    char *remaining = strtok(NULL, "");
                    if (externDefinition(remaining, countLine) == 0)
                    {
                        uncorrect = 1;
                    }
                    addL(countLine, 0);
                }
                /* Entry*/
                else if (strcmp(token, ".entry") == 0)
                {
                    char *remaining = strtok(NULL, "");
                    if (entryDefinition(remaining, countLine, 1) == 0)
                    {
                        uncorrect = 1;
                    }
                    addL(countLine, 0);
                }
                /* Operation list*/
                else if (findOperation(token) + 1)
                {
                    int numOper = findOperation(token);
                    char *remaining = strtok(NULL, "");
                    if (addOperation(remaining, numOper, countLine, newSymbolName) == 0)
                    {
                        uncorrect = 1;
                    }
                }
            }
            /* Uncorrect introduction*/
            else if (strcmp(token, ";"))
            {
                fprintf(stderr, "Error: line %d introduction? name incorrect.\n", countLine);
                uncorrect = 1;
            }
        }
        fprintf(stderr, "line %d count word %d.\n", countLine, l[countLine - 1].wordCounter);
        free(newSymbolName);
        newSymbolName = NULL;
    }
    fclose(sourceFile);
    if (uncorrect == 1)
    {
        fprintf(stderr, "Error: Incorrecrt file was recieved.\n");
        exit(EXIT_FAILURE);
    }
}

/* Second pass*/
void secondPass(const char *sourceFilename)
{
    fprintf(stderr, "second pass.\n"); /* check*/
    FILE *sourceFile = fopen(sourceFilename, "r");
    if (!sourceFile)
    {
        perror("Error opening input file");
        return;
    }
    char line[MAX_LINE_LENGTH];
    char lineToCheck[MAX_LINE_LENGTH];
    int uncorrect = 0;
    int countLine = 0;
    char codeBin[CODE_SEGMENT_SIZE];
    char *newSymbolName = NULL;
    int countAdress = 0;

    while (fgets(line, MAX_LINE_LENGTH, sourceFile))
    {
        strcpy(lineToCheck, line);
        countLine++;
        char *token = strtok(lineToCheck, " \t\n");
        /* label*/
        if (token)
        {
            newSymbolName = labelDefinition(token); /* Check if it is a label*/
            if ((newSymbolName) != NULL)
            {                                  /* Label definition*/
                token = strtok(NULL, " \t\n"); /* Next field*/
            }
        }
        /* check the first operation*/
        if (token)
        {
            /* Check if the definition has done*/
            if ((strcmp(token, ";") == 0) || (strcmp(token, ".data") == 0) || (strcmp(token, ".string") == 0) || (strcmp(token, ".extern") == 0))
            {
                continue;
            }
            /* Entry*/
            else if (strcmp(token, ".entry") == 0)
            {
                char *remaining = strtok(NULL, "");
                if (entryDefinition(remaining, countLine, 0) == 0)
                {
                    uncorrect = 1;
                }
            }
            /* Operation list*/
            else if (findOperation(token) + 1)
            {
                char *remaining = strtok(NULL, "");
                if (updateOparand(remaining, countLine, countAdress) == 0)
                {
                    uncorrect = 1;
                }
            }
        }
        countAdress += l[countLine - 1].wordCounter;
    }

    if (uncorrect == 1)
    {
        fprintf(stderr, "Error: Incorrecrt file was recieved.\n");
        exit(EXIT_FAILURE);
    }
    fclose(sourceFile);
}

/* Build output files*/
void buildOutputFiles(const char *sourceFilename)
{

    fprintf(stderr, "build out put files.\n"); /* check*/
    FILE *sourceFile = fopen(sourceFilename, "r");
    if (!sourceFile)
    {
        perror("Error opening input file");
        return;
    }

    /* Create .ob file*/
    char objectFilename[MAX_LINE_LENGTH];
    strcpy(objectFilename, changeNameOfFile(sourceFilename, ".ob"));

    FILE *obFile = fopen(objectFilename, "w");
    if (obFile == NULL)
    {
        perror("Error creating .o file");
        return;
    }

    /* Create .ext file*/
    char externFilename[MAX_LINE_LENGTH];
    strcpy(externFilename, changeNameOfFile(sourceFilename, ".ext"));

    FILE *extFile = fopen(externFilename, "w");
    if (extFile == NULL)
    {
        perror("Error creating .ext file");
        fclose(obFile);
        return;
    }
    int externExist = 0;

    /* Create .ent file*/
    char entryFilename[MAX_LINE_LENGTH];
    strcpy(entryFilename, changeNameOfFile(sourceFilename, ".ent"));

    FILE *entFile = fopen(entryFilename, "w");
    if (entFile == NULL)
    {
        perror("Error creating .ent file");
        return;
    }
    int entryExist = 0;

    /* Write to .ob file and to .ext file*/
    fprintf(obFile, "%d \t %d \n", IC, DC);
    for (int i = 0; i < symbolCount; i++)
    {
        Symbol *sym = &symbols[i];
        int codeBinInDecimal = binaryToDecimal(sym->code);
        fprintf(obFile, "%04d \t %05o \n", sym->address, codeInDecimal);
       /* fprintf(obFile, "%s\n", sym->code); check the  binary code*/
        if (sym->isExtern)
        {
            externExist = 1;
            fprintf(extFile, "%s \t %04d \n", sym->externName, sym->address);
            fflush(extFile);
        }
    }

    /* write to .ent file*/
    for (int i = 0; i < labelCount; i++)
    {
        Symbol *sym = &symbols[symbolTable[i].count];
        if (sym->isEntry)
        {
            entryExist = 1;
            fprintf(entFile, "%s \t %04d \n", sym->symbol, sym->address);
            fflush(entFile);
        }
    }

    fclose(extFile);
    fclose(entFile);
    fclose(obFile);

    /* no entryes*/
    if (!entryExist)
    {
        remove(entryFilename);
    }

    /* no externs*/
    if (!externExist)
    {
        remove(externFilename);
    }
}
