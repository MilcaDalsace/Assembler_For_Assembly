#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <math.h>
#include <ctype.h>
#include "transition.h"
#include "function.h"

// First pass
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
    char code[CODE_SEGMENT_SIZE];
    int countWord = 0;
    char *newSymbolName = NULL;

    while (fgets(line, MAX_LINE_LENGTH, sourceFile))
    {
        countLine++;
        strcpy(lineToCheck, line);
        char *token = strtok(lineToCheck, " \t\n");
        if (token)
        {
            char *newSymbolName = labelDefinition(token); // Check if it is a new label
            if ((newSymbolName))
            { // Label definition
                if (isNameRestricted(newSymbolName))
                { // Check if it is a restricted name
                    fprintf(stderr, "Error: line %d %s  is restricted name.\n", countLine, newSymbolName);
                    uncorrect = 1;
                }
                else
                {                                                                                // A new correct label
                    token = strtok(NULL, " \t\n");                                               // Next field
                    fprintf(stderr, "line: %d %s naw symbol name.\n", countLine, newSymbolName); // check
                }
            }
            if (token)
            {
                // Data
                if (strcmp(token, ".data") == 0)
                {
                    char *remaining = strtok(NULL, "");
                    if(addData(remaining,countLine,newSymbolName)==0){
                        uncorrect=1;
                    }
                    /*char *remaining = strtok(NULL, "");
                    if (remaining != NULL && correctCommas(remaining))
                    {
                        token = strtok(remaining, ", \t\n");
                        while (token != NULL)
                        {
                            if (token && isNumber(token))
                            {
                                int value = atoi(token);
                                strcpy(code, decimalToBinary(value, CODE_SEGMENT_SIZE - 1));
                                addSymbol(newSymbolName, NULL, code, 1, 0, 0);                       // the first word
                                fprintf(stderr, " line: %d add %d  to symbol.\n", countLine, value); // check
                                newSymbolName = NULL;
                                countWord++; // add 1 L
                            }
                            else
                            {
                                fprintf(stderr, "Error: line %d not a number \n", countLine);
                                uncorrect = 1;
                            }
                            token = strtok(NULL, " \t\n");
                        }
                    }
                    else
                    {
                        fprintf(stderr, "line %d \n", countLine);
                        uncorrect = 1;
                    }
                    DC += countWord;*/
                    // String
                }
                else if (strcmp(token, ".string") == 0)
                {
                    token = strtok(NULL, " \t\n");

                    if (isCorrectString(token) == 0)
                    {
                        fprintf(stderr, "Error: line %d Missing quotation marks.\n", countLine);
                        uncorrect = 1;
                    }
                    else
                    {
                        int length = strlen(token);
                        fprintf(stderr, " line: %d %d length.\n", countLine, length);
                        char charToAdd = token[1];
                        int value = charToAdd;                                                   // ASCII value
                        strcpy(code, decimalToBinary(value, CODE_SEGMENT_SIZE - 1));             // Create binary code
                        addSymbol(newSymbolName, NULL, code, 1, 0, 0);                           // The first word
                        fprintf(stderr, " line: %d add %d  to symbol.\n", countLine, charToAdd); // check
                        countWord++;
                        for (int i = 2; i < length - 1; i++)
                        {
                            charToAdd = token[i];
                            value = charToAdd;                                                      // ASCII value
                            strcpy(code, decimalToBinary(value, CODE_SEGMENT_SIZE - 1));            // Create binary code
                            addSymbol(NULL, NULL, code, 1, 0, 0);                                   // All the next chars
                            fprintf(stderr, " line: %d add %d to symbol.\n", countLine, charToAdd); // check
                            countWord++;
                        }
                        strcpy(code, decimalToBinary(0, CODE_SEGMENT_SIZE - 1));    // Create binary code
                        addSymbol(NULL, NULL, code, 1, 0, 0);                       // Add /0 at the end
                        fprintf(stderr, " line: %d add O to symbol.\n", countLine); // check
                        countWord++;

                        token = strtok(NULL, " \t\n");
                        if (token)
                        {
                            fprintf(stderr, "Error: line %d Additional characters at the line.\n", countLine);
                            uncorrect = 1;
                        }
                        DC += countWord;
                    }
                    // Extern
                }
                else if (strcmp(token, ".extern") == 0)
                {
                    token = strtok(NULL, " \t\n");
                    while (token != NULL)
                    {
                        addExtern(token);                                                            // Add extern
                        fprintf(stderr, " line: %d %s add extern.\n", countLine, findExtern(token)); // check
                        token = strtok(NULL, " \t\n");
                    }
                    // Entry
                }
                else if (strcmp(token, ".entry") == 0)
                {
                    token = strtok(NULL, " \t\n");
                    if (token)
                    {
                        if (findLabbel(token) + 1)
                        {
                            Symbol *sym = &symbols[findLabbel(token)];
                            if (sym)
                            {
                                sym->isEntry = 1;
                                fprintf(stderr, " line: %d %s entry andicated.\n", countLine, token);
                            }
                        }
                        else
                        {
                            fprintf(stderr, " line: %d %s not yet found in labbels.\n", countLine, token);
                        }

                        token = strtok(NULL, " \t\n");
                        if (token)
                        {
                            fprintf(stderr, "Error: line %d Additional characters at the line.\n", countLine);
                            uncorrect = 1;
                        }
                    }
                    else
                    {
                        fprintf(stderr, "Error: line %d Labbel name is missing.\n", countLine);
                        uncorrect = 1;
                    }
                    // Operation list
                }
                else if (findOperation(token) + 1)
                {
                    char *remaining = strtok(NULL, "");
                    fprintf(stderr, " line %d %s %s is opration to add.\n", countLine, remaining, token); // check
                    if ((!remaining) || remaining && correctCommas(remaining))
                    {
                        char operand1[MAX_LINE_LENGTH];
                        char operand2[MAX_LINE_LENGTH];
                        int oper = findOperation(token);
                        token = strtok(remaining, " ,\t\n");

                        if (oper < 5)
                        { // The first group of instructions - receives 2 operands
                            if (token)
                            {
                                strcpy(operand1, token);
                                fprintf(stderr, " line: %d %s is operand1.\n", countLine, operand1); // check
                                token = strtok(NULL, " ,\t\n");
                            }
                            else
                            {
                                fprintf(stderr, "Error: line %d Missing operand1.\n", countLine);
                                uncorrect = 1;
                            }
                            if (token)
                            {
                                strcpy(operand2, token);
                                fprintf(stderr, " line %d %s isoperand2 add.\n", countLine, operand2); // check
                            }
                            else
                            {
                                fprintf(stderr, "Error: line %d Missing operand2.\n", countLine);
                                uncorrect = 1;
                            }

                            strcpy(code, miunOperation(oper, operand1, operand2));       // Create code to the operation
                            fprintf(stderr, "  %d %s code of.\n", oper, code);           // check
                            addSymbol(newSymbolName, NULL, code, 0, 0, 0);               // Add operation to the symbols
                            fprintf(stderr, "  %d %s code of operation.\n", oper, code); // check
                            countWord++;
                            if (isRegister(operand1) + 1 && isRegister(operand2) + 1)
                            {
                                strcpy(code, miunTwoRegister(operand1, operand2)); // Create 1 code to the registers
                                addSymbol(NULL, NULL, code, 0, 0, 0);
                                fprintf(stderr, "Miun two registers %s %s.\n", operand1, operand2); // check
                                countWord++;
                            }
                            else
                            {
                                strcpy(code, miunOperand(operand1, 1)); // Create code to operand1
                                if (findExtern(operand1))
                                {
                                    addSymbol(NULL, operand1, code, 0, 0, 1);                    // Add symbol with operand1
                                    fprintf(stderr, "  %d %s code of operand 1.\n", oper, code); // check
                                    countWord++;
                                }
                                else
                                {
                                    addSymbol(NULL, NULL, code, 0, 0, 0);                        // Add symbol with operand1
                                    fprintf(stderr, "  %d %s code of operand 2.\n", oper, code); // check
                                    countWord++;
                                }
                                strcpy(code, miunOperand(operand2, 0)); // Create code to operand2
                                if (findExtern(operand2))
                                {
                                    addSymbol(NULL, operand2, code, 0, 0, 1);          // Add symbol with operand2
                                    fprintf(stderr, "  %d %s code of.\n", oper, code); // check
                                    countWord++;
                                }
                                else
                                {
                                    addSymbol(NULL, NULL, code, 0, 0, 0);              // Add symbol with operand2
                                    fprintf(stderr, "  %d %s code of.\n", oper, code); // check
                                    countWord++;
                                }
                            }
                        }
                        else if (oper < 14)
                        { // The second group of instructions - receives 1 operand

                            // token = strtok(NULL, " \t\n");
                            if (token)
                            {
                                strcpy(operand1, token);

                                fprintf(stderr, " line: %d %s is operand1.\n", countLine, remaining); // check
                            }
                            else
                            {
                                fprintf(stderr, "Error: line %d Missing operand.\n", countLine);
                                uncorrect = 1;
                            }

                            strcpy(code, miunOperation(oper, NULL, operand1)); // Create code to the operation

                            addSymbol(newSymbolName, NULL, code, 0, 0, 0);               // Add operation to the symbols
                            fprintf(stderr, "  %d %s code of operation.\n", oper, code); // check
                            countWord++;
                            strcpy(code, miunOperand(operand1, 1));                     // Create code to operand1
                            fprintf(stderr, "  %d %s code of operand1.\n", oper, code); // check
                            if (findExtern(operand1))
                            {
                                addSymbol(NULL, operand1, code, 0, 0, 1); // Add symbol with operand2
                                countWord++;
                            }
                            else
                            {
                                addSymbol(NULL, NULL, code, 0, 0, 0); // Add symbol with operand1
                                countWord++;
                            }
                        }
                        else
                        { // The third group of instructions - doesn't receive any operands

                            strcpy(code, miunOperation(oper, NULL, NULL));               // Create code to the operation
                            fprintf(stderr, "  %d %s code of operation.\n", oper, code); // check
                            addSymbol(newSymbolName, NULL, code, 0, 0, 0);               // Add operation to the symbols
                            countWord++;
                        }

                        token = strtok(NULL, " \t\n");
                        if (token)
                        {
                            fprintf(stderr, "Error: line %d Additional operands.\n", countLine);
                            uncorrect = 1;
                        }
                    }
                    else
                    {
                        fprintf(stderr, "line %d.\n", countLine);
                        uncorrect = 1;
                    }

                    IC += countWord;
                }

                // Uncorrect introduction
            }
            else if (strcmp(token, ";"))
            {
                fprintf(stderr, "Error: line %d introduction? name incorrect.\n", countLine);
                uncorrect = 1;
            }
        }

        addL(countLine, countWord); // Updating L
        fprintf(stderr, "line %d count word %d.\n", countLine, l[countLine - 1].wordCounter);
        free(newSymbolName);
        newSymbolName = NULL;
        countWord = 0;
    }
    fclose(sourceFile);
    if (uncorrect == 1)
    {
        fprintf(stderr, "Error: Incorrecrt file was recieved.\n");
        exit(EXIT_FAILURE);
    }
}

// Second pass
void secondPass(const char *sourceFilename)
{
    fprintf(stderr, "second pass.\n"); // check
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
    char code[CODE_SEGMENT_SIZE];
    char *newSymbolName = NULL;
    int countAdress = 0;

    while (fgets(line, MAX_LINE_LENGTH, sourceFile))
    {
        strcpy(lineToCheck, line);
        countLine++;

        fprintf(stderr, "line: %d %d count adress.\n", countLine, countAdress); // check
        char *token = strtok(lineToCheck, " \t\n");
        if (token)
        {
            newSymbolName = labelDefinition(token); // Check if it is a label
            if ((newSymbolName) != NULL)
            {                                                                                // Label definition
                fprintf(stderr, "line: %d %s naw symbol name.\n", countLine, newSymbolName); // check
                token = strtok(NULL, " \t\n");                                               // Next field
            }
        }
        if (token)
        {
            if ((strcmp(token, ";") == 0) || (strcmp(token, ".data") == 0) || (strcmp(token, ".string") == 0) || (strcmp(token, ".extern") == 0))
            {
                fprintf(stderr, "line: %d %s was check in first pass.\n", countLine, token); // check
                continue;
                // Entry
            }
            else if (strcmp(token, ".entry") == 0)
            {
                fprintf(stderr, "line: %d %s in entry.\n", countLine, token); // check
                token = strtok(NULL, " \t\n");
                if (findLabbel(token) + 1)
                {
                    Symbol *sym = &symbols[findLabbel(token)];
                    if (sym)
                    { // If found a label
                        if (sym->isEntry == 0)
                        {                                                                                   // If not found in the first pass
                            fprintf(stderr, "line %d:  updatind %d to entry .\n", countLine, sym->address); // check
                            sym->isEntry = 1;
                        }
                    }
                    else
                    { // Not exist in the table of labels
                        fprintf(stderr, "Error: line %d label not exist.\n", countLine);
                        uncorrect = 1;
                    }
                }
                else
                {
                    fprintf(stderr, "Error: line %d Labbel name is missing.\n", countLine);
                    uncorrect = 1;
                }

                // Operation list
            }
            else if (findOperation(token) + 1)
            {
                token = strtok(NULL, " ,\t\n");
                fprintf(stderr, "line: %d %s Operation.\n", countLine, token); // check
                int i = 1;
                while (token != NULL)
                {
                    if (strcmp(symbols[countAdress + i].code, NOT_FOUND) == 0)
                    {                                                                                // Operand not found at the first pass
                        fprintf(stderr, "line: %d %s not found at first past.\n", countLine, token); // check

                        Symbol *sym = &symbols[countAdress + i];
                        strcpy(code, miunOperand(token, 1));
                        fprintf(stderr, "  %s %s code of.\n", token, code); // check
                        strncpy(sym->code, code, CODE_SEGMENT_SIZE);
                        if (findExtern(token))
                        {
                            sym->isExtern = 1;
                            strncpy(sym->externName, token, MAX_LABEL_NAME);
                        }
                        if (strcmp(code, NOT_FOUND) == 0)
                        {
                            fprintf(stderr, "Error: line %d label not found.\n", countLine);
                            uncorrect = 1;
                        }
                    }
                    i++;
                    token = strtok(NULL, " ,\t\n");
                }
            }
            countAdress += l[countLine - 1].wordCounter;
        }
    }

    if (uncorrect == 1)
    {
        fprintf(stderr, "Error: Incorrecrt file was recieved.\n");
        exit(EXIT_FAILURE);
    }
    fclose(sourceFile);
}

// Build output files
void buildOutputFiles(const char *sourceFilename)
{

    fprintf(stderr, "build out put files.\n"); // check
    FILE *sourceFile = fopen(sourceFilename, "r");
    if (!sourceFile)
    {
        perror("Error opening input file");
        return;
    }

    // Create .ob file
    char objectFilename[MAX_LINE_LENGTH];
    strcpy(objectFilename, sourceFilename);
    char *dot = strrchr(objectFilename, '.');
    if (dot != NULL)
    {
        *dot = '\0'; // מחיקת הסיומת הקיימת
    }
    strcat(objectFilename, ".ob");

    FILE *obFile = fopen(objectFilename, "w");
    if (obFile == NULL)
    {
        perror("Error creating .o file");
        return;
    }

    // Create .ext file
    char externFilename[MAX_LINE_LENGTH];
    strcpy(externFilename, sourceFilename);
    dot = strrchr(externFilename, '.');
    if (dot != NULL)
    {
        *dot = '\0'; // מחיקת הסיומת הקיימת
    }
    strcat(externFilename, ".ext");

    FILE *extFile = fopen(externFilename, "w");
    if (extFile == NULL)
    {
        perror("Error creating .ext file");
        fclose(obFile);
        return;
    }
    int externExist = 0;

    // Create .ent file
    char entryFilename[MAX_LINE_LENGTH];
    strcpy(entryFilename, sourceFilename);
    dot = strrchr(entryFilename, '.');
    if (dot != NULL)
    {
        *dot = '\0';
    }

    strcat(entryFilename, ".ent");

    FILE *entFile = fopen(entryFilename, "w");
    if (entFile == NULL)
    {
        perror("Error creating .ent file");
        return;
    }

    int entryExist = 0;

    fprintf(stderr, "build files.\n");                    // check
    fprintf(stderr, "num of symbol: %d.\n", symbolCount); // check
    fprintf(stderr, "IC= %d DC= %d.\n", IC, DC);          // check

    // Write to .ob file and to .ext file
    fprintf(obFile, "%d \t %d \n", IC, DC);
    for (int i = 0; i < symbolCount; i++)
    {
        Symbol *sym = &symbols[i];
        int codeInDecimal = binaryToDecimal(sym->code);
        fprintf(obFile, "%04d \t %05o \n", sym->address, codeInDecimal);
        fprintf(obFile, "%s\n", sym->code);                                       // check the  binary code
        fprintf(stderr, "is extern: %s %04d .\n", sym->externName, sym->address); // check
        if (sym->isExtern)
        {

            externExist = 1;

            fprintf(extFile, "%s \t %04d \n", sym->externName, sym->address);
            fflush(extFile);
        }
    }

    // write to .ent file
    for (int i = 0; i < labelCount; i++)
    {
        Symbol *sym = &symbols[symbolTable[i].count];
        fprintf(stderr, "sym: %d %s is entry: %d.\n", sym->address, sym->symbol, sym->isEntry); // check
        if (sym->isEntry)
        {
            entryExist = 1;
            fprintf(stderr, "sym: %s %04d .\n", sym->symbol, sym->address); // check

            fprintf(entFile, "%s \t %04d \n", sym->symbol, sym->address);
            fflush(entFile);
        }
    }

    fclose(extFile);
    fclose(entFile);
    fclose(obFile);

    // no entryes
    if (!entryExist)
    {
        remove(entryFilename);
    }

    // no externs
    if (!externExist)
    {
        remove(externFilename);
    }
}

// end
