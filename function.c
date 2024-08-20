#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <math.h>
#include <ctype.h>
#include "function.h"
#include "preAssembler.h"

/* Constant strings for addressing methods*/
const char IMMEDIATE_ADDRESS[] = "0001";
const char DIRECT_ADDRESS[] = "0010";
const char INDIRECT_HOARD_ADDRESS[] = "0100";
const char DIRECT_HOARD_ADDRESS[] = "1000";
const char ABSOLUTE[] = "100";
const char RELOCATABLE[] = "010";
const char EXTERNAL[] = "001";
const char ZERO[] = "0000";
const char NOT_FOUND[] = "labbelnotfound!";
const int registers[] = {8, 7, 6, 5, 4, 3, 2, 1};

extern char* strdup(const char*);

Symbol *symbols = NULL;
int symbolCount = 0;

SymbolTable *symbolTable = NULL;
int labelCount = 0;

External *externs = NULL;
int externCount = 0;

L *l = NULL;

int IC = 0;
int DC = 0;

int addData(const char *line, int countLine, const char *newSymbolName)
{
    int countWord = 0;
    char codeBin[CODE_SEGMENT_SIZE];
    char *lineCopy;
    if (line)
    {
        lineCopy = strdup(line);
    }
    else
    {
        lineCopy = NULL;
    }
    if (lineCopy != NULL && correctCommas(lineCopy))
    {
        char *token = strtok(lineCopy, ", \t\n");
        while (token != NULL)
        {
            if (token && isNumber(token))
            {
                int value = atoi(token);
                strcpy(codeBin, decimalToBinary(value, CODE_SEGMENT_SIZE - 1));
                addSymbol(newSymbolName, NULL, codeBin, 1, 0, 0);                       /* the first word*/
                newSymbolName = NULL;
                countWord++; /* add 1 L*/
            }
            else
            {
                fprintf(stderr, "Error: line %d not a number \n", countLine);
                return 0;
            }
            token = strtok(NULL, " \t\n");
        }
    }
    else
    {
        fprintf(stderr, "line %d \n", countLine);
        return 0;
    }
    DC += countWord;
    addL(countLine, countWord);
    return 1;
}

int addString(const char *line, int countLine, const char *newSymbolName)
{
    int countWord = 0;
    char codeBin[CODE_SEGMENT_SIZE];
    char *lineCopy;
    if (line)
    {
        lineCopy = strdup(line);
    }
    else
    {
        lineCopy = NULL;
    }
    char *token = strtok(lineCopy, " \t\n");
    if (isCorrectString(token) == 0)
    {
        fprintf(stderr, "Error: line %d Missing quotation marks.\n", countLine);
        return 0;
    }
    else
    {
        int length = strlen(token);
        fprintf(stderr, " line: %d %d length.\n", countLine, length);
        char charToAdd = token[1];
        int value = charToAdd;                                                   /* ASCII value*/
        strcpy(codeBin, decimalToBinary(value, CODE_SEGMENT_SIZE - 1));             /* Create binary code*/
        addSymbol(newSymbolName, NULL, codeBin, 1, 0, 0);                           /* The first word*/
        countWord++;
        for (int i = 2; i < length - 1; i++)
        {
            charToAdd = token[i];
            value = charToAdd;                                                      /* ASCII value*/
            strcpy(codeBin, decimalToBinary(value, CODE_SEGMENT_SIZE - 1));            /* Create binary code*/
            addSymbol(NULL, NULL, codeBin, 1, 0, 0);                                   /* All the next chars*/
            countWord++;
        }
        strcpy(codeBin, decimalToBinary(0, CODE_SEGMENT_SIZE - 1));    /* Create binary codeBin*/
        addSymbol(NULL, NULL, codeBin, 1, 0, 0);                       /* Add /0 at the end*/
        countWord++;

        token = strtok(NULL, " \t\n");
        if (token)
        {
            fprintf(stderr, "Error: line %d Additional characters at the line.\n", countLine);
            return 0;
        }
        DC += countWord;
        addL(countLine, countWord);
        return 1;
    }
}

int externDefinition(const char *line, int countLine)
{
    char *remaining = strdup(line);
    char *lineCopy;
    if (line)
    {
        lineCopy = strdup(line);
    }
    else
    {
        lineCopy = NULL;
    }
    if (remaining && correctCommas(remaining))
    {
        char *token = strtok(lineCopy, " \t\n");
        while (token != NULL)
        {
            if (isNameRestricted(token))
            {
                fprintf(stderr, "Error: line %d is restricted name\n", countLine);
                return 0;
            }
            else
            {
                addExtern(token); /* Add extern*/
                token = strtok(NULL, " \t\n");
            }
        }
        return 1;
    }
    else
    {
        fprintf(stderr, "Error: line %d the Extern definition is uncorrect.\n", countLine);
        return 0;
    }
}

int entryDefinition(const char *line, int countLine, int isFirstPass)
{
    char *lineCopy;
    if (line)
    {
        lineCopy = strdup(line);
    }
    else
    {
        lineCopy = NULL;
    }
    char *token = strtok(lineCopy, " \t\n");
    if (token)
    {
        if (findLabbel(token) + 1)
        {
            Symbol *sym = &symbols[findLabbel(token)];
            if (sym && sym->isEntry == 0)
            {
                sym->isEntry = 1;
            }
        }
        else if (isFirstPass)
        {
            fprintf(stderr, " line: %d %s not yet found in labbels.\n", countLine, token);
        }
        else
        {
            fprintf(stderr, "Error: line %d label not exist.\n", countLine);
            return 0;
        }

        token = strtok(NULL, " \t\n");
        if (token)
        {
            fprintf(stderr, "Error: line %d Additional characters at the line.\n", countLine);
            return 0;
        }
    }
    else
    {
        fprintf(stderr, "Error: line %d Labbel name is missing.\n", countLine);
        return 0;
    }
    return 1;
}

int addOperation(const char *line, int numOper, int countLine, const char *newSymbolName)
{
    char *lineCopy;
    if (line)
    {
        lineCopy = strdup(line);
    }
    else
    {
        lineCopy = NULL;
    }

    if ((!line) || (lineCopy && correctCommas(lineCopy)))
    {
        if (numOper < 5)
        { /* The first group of instructions - receives 2 operands*/
            if (!operationWithTwoOperand(lineCopy, newSymbolName, numOper, countLine))
            {
                free(lineCopy);
                return 0;
            }
        }
        else if (numOper < 14)
        { /* The second group of instructions - receives 1 operand*/
            if (!operationWithOneOperand(lineCopy, newSymbolName, numOper, countLine))
            {
                free(lineCopy);
                return 0;
            }
        }
        else
        { /* The third group of instructions - doesn't receive any operands*/
            if (!operationWithoutOperand(lineCopy, newSymbolName, numOper, countLine))
            {
                free(lineCopy);
                return 0;
            }
        }
    }
    else
    {
        fprintf(stderr, "line %d.\n", countLine);
        free(lineCopy);
        return 0;
    }
    free(lineCopy);
    return 1;
}

int updateOparand(const char *line, int countLine, int countAdress)
{
    char *lineCopy;
    if (line)
    {
        lineCopy = strdup(line);
    }
    else
    {
        lineCopy = NULL;
    }
    char *token = strtok(lineCopy, " ,\t\n");
    char codeBin[CODE_SEGMENT_SIZE];
    int i = 1;
    while (token != NULL)
    {
        int simbolAddress = countAdress + i;
        if (strcmp(symbols[simbolAddress].code, NOT_FOUND) == 0)
        { /* Operand not found at the first pass*/
            Symbol *sym = &symbols[simbolAddress];
            strcpy(codeBin, miunOperand(token, 1));
            strncpy(sym->code, codeBin, CODE_SEGMENT_SIZE);
            if (findExtern(token))
            {
                sym->isExtern = 1;
                strncpy(sym->externName, token, MAX_LABEL_NAME);
            }
            if (strcmp(codeBin, NOT_FOUND) == 0)
            {
                fprintf(stderr, "Error: line %d label not found.\n", countLine);
                return 0;
            }
        }
        i++;
        token = strtok(NULL, " ,\t\n");
    }
    return 1;
}

char *changeNameOfFile(const char *sourceFileName, const char *fileNameEnding)
{
      char *updateFileName = malloc(MAX_LINE_LENGTH); // הקצאת זיכרון דינמית
 
    char *dot = strrchr(sourceFileName, '.');
    strcpy(updateFileName, sourceFileName);
    dot = strrchr(updateFileName, '.');
    if (dot != NULL)
    {
        *dot = '\0';
    }

    strcat(updateFileName, fileNameEnding);
    return updateFileName;
}

int operationWithTwoOperand(const char *line, const char *newSymbolName, int numOper, int countLine)
{
    char codeBin[CODE_SEGMENT_SIZE];
    char operand1[MAX_LINE_LENGTH];
    char operand2[MAX_LINE_LENGTH];
    char *lineCopy;
    if (line)
    {
        lineCopy = strdup(line);
    }
    else
    {
        lineCopy = NULL;
    }
    char *token = strtok(lineCopy, " ,\t\n");
    int countWord = 0;

    if (token)
    {
        strcpy(operand1, token);
        token = strtok(NULL, " ,\t\n");
    }
    else
    {
        fprintf(stderr, "Error: line %d Missing operand1.\n", countLine);
        return 0;
    }
    if (token)
    {
        strcpy(operand2, token);
    }
    else
    {
        fprintf(stderr, "Error: line %d Missing operand2.\n", countLine);
        return 0;
    }

    strcpy(codeBin, miunOperation(numOper, operand1, operand2)); /* Create code to the operation*/
    addSymbol(newSymbolName, NULL, codeBin, 0, 0, 0);            /* Add operation to the symbols*/

    countWord++;
    if (isRegister(operand1) + 1 && isRegister(operand2) + 1)
    {
        strcpy(codeBin, miunTwoRegister(operand1, operand2)); /* Create 1 code to the registers*/
        addSymbol(NULL, NULL, codeBin, 0, 0, 0);
        countWord++;
    }
    else
    {
        strcpy(codeBin, miunOperand(operand1, 1)); /* Create code to operand1*/
        if (findExtern(operand1))
        {
            addSymbol(NULL, operand1, codeBin, 0, 0, 1); /* Add symbol with operand1*/
            countWord++;
        }
        else
        {
            addSymbol(NULL, NULL, codeBin, 0, 0, 0); /* Add symbol with operand1*/
            countWord++;
        }
        strcpy(codeBin, miunOperand(operand2, 0)); /* Create code to operand2*/
        if (findExtern(operand2))
        {
            addSymbol(NULL, operand2, codeBin, 0, 0, 1); /* Add symbol with operand2*/
            countWord++;
        }
        else
        {
            addSymbol(NULL, NULL, codeBin, 0, 0, 0); /* Add symbol with operand2*/
            countWord++;
        }
    }
    token = strtok(NULL, " \t\n");
    if (token)
    {
        fprintf(stderr, "Error: line %d Additional operands.\n", countLine);
        return 0;
    }
    IC += countWord;
    addL(countLine, countWord);
    return 1;
}

int operationWithOneOperand(const char *line, const char *newSymbolName, int numOper, int countLine)
{
    char codeBin[CODE_SEGMENT_SIZE];
    char operand1[MAX_LINE_LENGTH];
    char *lineCopy;
    if (line)
    {
        lineCopy = strdup(line);
    }
    else
    {
        lineCopy = NULL;
    }
    char *token = strtok(lineCopy, " ,\t\n");
    int countWord = 0;
    if (token)
    {
        strcpy(operand1, token);
    }
    else
    {
        fprintf(stderr, "Error: line %d Missing operand.\n", countLine);
        return 0;
    }

    strcpy(codeBin, miunOperation(numOper, NULL, operand1)); /* Create code to the operation*/

    addSymbol(newSymbolName, NULL, codeBin, 0, 0, 0); /* Add operation to the symbols*/
    countWord++;
    strcpy(codeBin, miunOperand(operand1, 1)); /* Create code to operand1*/
    if (findExtern(operand1))
    {
        addSymbol(NULL, operand1, codeBin, 0, 0, 1); /* Add symbol with operand2*/
        countWord++;
    }
    else
    {
        addSymbol(NULL, NULL, codeBin, 0, 0, 0); /* Add symbol with operand1*/
        countWord++;
    }
    token = strtok(NULL, " \t\n");
    if (token)
    {
        fprintf(stderr, "Error: line %d Additional operands.\n", countLine);
        return 0;
    }
    IC += countWord;
    addL(countLine, countWord);
    return 1;
}

int operationWithoutOperand(const char *line, const char *newSymbolName, int numOper, int countLine)
{
    char codeBin[CODE_SEGMENT_SIZE];
    /* char *token = strtok(line, " ,\t\n");*/
    int countWord = 0;
    strcpy(codeBin, miunOperation(numOper, NULL, NULL)); /* Create code to the operation*/
    addSymbol(newSymbolName, NULL, codeBin, 0, 0, 0);    /* Add operation to the symbols*/
    countWord++;
    /* token = strtok(NULL, " \t\n");*/
    if (line)
    {
        fprintf(stderr, "Error: line %d Additional operands.\n", countLine);
        return 0;
    }
    IC += countWord;
    addL(countLine, countWord);
    return 1;
}

int checkIfLabbleIsData(const char *label)
{
    int labelAddress = findLabbel(label);
    if (labelAddress + 1)
    {
        Symbol *sym = &symbols[labelAddress];
        if (sym->isData == 0)
        {
            return 0;
        }
    }
    return 1;
}
/* Restricted name*/
int isNameRestricted(const char *name)
{
    if (isRegister(name) + 1)
    {
        return 1;
    }
    else if (findOperation(name) + 1)
    {
        return 1;
    }
    else if (findMacro(name))
    {
        return 1;
    }
    else if (findLabbel(name) + 1)
    {
        return 1;
    }
    return 0;
}

/* find operation*/
int findOperation(const char *name)
{
    const char *operations[] = {"mov", "cmp", "add", "sub", "lea", "clr", "not", "inc", "dec", "jmp", "bne", "red", "prn", "jsr", "rts", "stop"};
    int restrictedNamesCount = 16;
    for (int i = 0; i < restrictedNamesCount; i++)
    {
        if (strcmp(name, operations[i]) == 0)
        {
            return i;
        }
    }
    return -1;
}

/* Is register*/
int isRegister(const char *name)
{
    const char *registers[] = {"r0", "r1", "r2", "r3", "r4", "r5", "r6", "r7"};
    int registerNum = 8;
    char temp[strlen(name) + 1];
    strcpy(temp, name);

    if (temp[0] == '*')
    {
        memmove((char *)temp, temp + 1, strlen(temp));
    }
    for (int i = 0; i < registerNum; i++)
    {
        if (strcmp(temp, registers[i]) == 0)
        {
            return i;
        }
    }
    return -1;
}

/* Find method of addressing*/
char *findsMethodOfAddressing(const char *operand)
{

    if (operand)
    {
        if (operand[0] == '#')
        { /* a number*/
            if (!isNumber(operand + 1))
                return NULL; /*!*/
            return strdup(IMMEDIATE_ADDRESS);
        }
        else if (findLabbel(operand) + 1)
        { /* label*/
            return strdup(DIRECT_ADDRESS);
        }
        else if (findExtern(operand))
        { /* external label*/
            return strdup(DIRECT_ADDRESS);
        }
        else if ((isRegister(operand) + 1) && (operand[0] == '*'))
        { /* A pointer to a register*/
            return strdup(INDIRECT_HOARD_ADDRESS);
        }
        else if (isRegister(operand) + 1)
        { /* A register*/
            return strdup(DIRECT_HOARD_ADDRESS);
        }
        else
        { /* label no definition yet*/
            return strdup(DIRECT_ADDRESS);
        }
    }
    return strdup(ZERO);
}

/* Miun operation*/
char *miunOperation(int num, const char *operand1, const char *operand2)
{
    static char codeBin[CODE_SEGMENT_SIZE];
    char are[ARE_SIZE];

    char *opCode = decimalToBinary(num, CODE_SIZE);
    strcpy(are, ABSOLUTE);
    char *addressingMethod1 = findsMethodOfAddressing(operand1);
    char *addressingMethod2 = findsMethodOfAddressing(operand2);

    snprintf(codeBin, CODE_SEGMENT_SIZE, "%s%s%s%s", opCode, addressingMethod1, addressingMethod2, are);
    return codeBin;
}

/* Miun operand*/
char *miunOperand(const char *operand, int firstOperand)
{
    static char codeBin[CODE_SEGMENT_SIZE];
    char address[CODE_EE_SIZE];
    char addressingMethod[CODE_SIZE];
    char are[ARE_SIZE];

    if (operand[0] == '#')
    { /* a number*/
        char num[strlen(operand) - 1];
        strncpy(num, &operand[1], strlen(operand) - 1);
        num[strlen(operand) - 1] = '\0';
        int value = atoi(num);
        strcpy(address, decimalToBinary(value, CODE_SIZE * 3));
        strcpy(are, ABSOLUTE);
    }
    else if (findLabbel(operand) + 1)
    { /* label*/
        Symbol *sym = &symbols[findLabbel(operand)];
        int value = sym->address;
        strcpy(address, decimalToBinary(value, CODE_SIZE * 3));
        strcpy(are, RELOCATABLE);
    }
    else if (findExtern(operand))
    { /* external label*/
        strcpy(address, decimalToBinary(0, CODE_SIZE * 3));
        strcpy(are, EXTERNAL);
    }
    else if (isRegister(operand) + 1)
    { /* A pointer to a register*/
        int num = isRegister(operand);
        strcpy(addressingMethod, decimalToBinary(registers[num], CODE_SIZE));
        if (firstOperand)
        {
            snprintf(address, sizeof(address), "%s%s%s", ZERO, addressingMethod, ZERO);
        }
        else
        {
            snprintf(address, sizeof(address), "%s%s%s", ZERO, ZERO, addressingMethod);
        }
        strcpy(are, ABSOLUTE);
    }
    else
    { /* not correct*/
        return strdup(NOT_FOUND);
    }
    snprintf(codeBin, CODE_SEGMENT_SIZE, "%s%s", address, are);
    return codeBin;
}

/* Miun two registers*/
char *miunTwoRegister(const char *operand1, const char *operand2)
{
    static char codeBin[CODE_SEGMENT_SIZE];
    char addressingMethod1[CODE_SIZE + 1];
    char addressingMethod2[CODE_SIZE + 1];

    int num = isRegister(operand1);
    strcpy(addressingMethod1, decimalToBinary(registers[num], CODE_SIZE));
    num = isRegister(operand2);
    strcpy(addressingMethod2, decimalToBinary(registers[num], CODE_SIZE));

    snprintf(codeBin, CODE_SEGMENT_SIZE, "%s%s%s%s", ZERO, addressingMethod1, addressingMethod2, ABSOLUTE);
    return codeBin;
}

/* Add symbol*/
void addSymbol(const char *symbolName, const char *externName, const char *codeBin, const int isData, const int isEntry, const int isExtern)
{
    Symbol *temp = realloc(symbols, (symbolCount + 1) * sizeof(Symbol));
    if (!temp)
    {
        fprintf(stderr, "Error: Memory allocation failed.\n");
        exit(EXIT_FAILURE);
    }
    symbols = temp;
    symbolCount++;

    if (symbolName != NULL)
    {
        strncpy(symbols[symbolCount - 1].symbol, symbolName, MAX_LINE_LENGTH - 1);
        symbols[symbolCount - 1].symbol[MAX_LINE_LENGTH - 1] = '\0';
    }
    else
    {
        symbols[symbolCount - 1].symbol[0] = '\0';
    }

    if (externName != NULL)
    {
        strncpy(symbols[symbolCount - 1].externName, externName, MAX_LABEL_NAME - 1);
        symbols[symbolCount - 1].externName[MAX_LABEL_NAME - 1] = '\0';
    }
    else
    {
        symbols[symbolCount - 1].externName[0] = '\0';
    }

    strncpy(symbols[symbolCount - 1].code, codeBin, CODE_SEGMENT_SIZE - 1);
    symbols[symbolCount - 1].code[CODE_SEGMENT_SIZE - 1] = '\0';

    symbols[symbolCount - 1].address = symbolCount + 99; /* Not sure about the calculation, verify the correct address*/
    symbols[symbolCount - 1].isData = isData;
    symbols[symbolCount - 1].isEntry = isEntry;
    symbols[symbolCount - 1].isExtern = isExtern;

    if (symbolName != NULL)
    { /* Add label if present*/
        addLabbel(symbolName, symbolCount - 1);
    }
}

/* Add label to symbol table*/
void addLabbel(const char *name, const int count)
{
    SymbolTable *temp = realloc(symbolTable, (labelCount + 1) * sizeof(SymbolTable));
    if (!temp)
    {
        fprintf(stderr, "Error: Memory allocation failed.\n");
        exit(EXIT_FAILURE);
    }
    symbolTable = temp;
    labelCount++;
    symbolTable[labelCount - 1].name = strdup(name);
    if (!symbolTable[labelCount - 1].name)
    {
        fprintf(stderr, "Error: Memory allocation failed.\n");
        exit(EXIT_FAILURE);
    }
    symbolTable[labelCount - 1].count = count;
}

/* Add external symbol*/
void addExtern(const char *name)
{
    External *temp = realloc(externs, (externCount + 1) * sizeof(External));
    if (!temp)
    {
        fprintf(stderr, "Error: Memory allocation failed.\n");
        exit(EXIT_FAILURE);
    }
    externs = temp;
    externCount++;
    externs[externCount - 1].name = strdup(name);
    if (!externs[externCount - 1].name)
    {
        fprintf(stderr, "Error: Memory allocation failed.\n");
        exit(EXIT_FAILURE);
    }
}

/* Add line count*/
void addL(int counteLine, int wordCounter)
{
    L *temp = realloc(l, counteLine * sizeof(L));
    if (!temp)
    {
        fprintf(stderr, "Error: Memory allocation failed.\n");
        exit(EXIT_FAILURE);
    }
    l = temp;
    l[counteLine - 1].wordCounter = wordCounter;
}

/* Label definition*/
char *labelDefinition(const char *token)
{
    int length = strlen(token);
    if (token[length - 1] == ':')
    {
        char *newSymbolName = malloc(length);
        if (!newSymbolName)
        {
            fprintf(stderr, "Error: Memory allocation failed.\n");
            exit(EXIT_FAILURE);
        }
        strncpy(newSymbolName, token, length - 1); /* Copy the name without ':'*/
        newSymbolName[length - 1] = '\0';
        return newSymbolName;
    }
    return NULL;
}

/* Find label*/
int findLabbel(const char *symbolName)
{
    for (int i = 0; i < symbolCount; i++)
    {
        if (strcmp(symbols[i].symbol, symbolName) == 0)
        {
            return i;
        }
    }
    return -1;
}

/* Find external symbol*/
char *findExtern(const char *externName)
{
    for (int i = 0; i < externCount; i++)
    {
        if (strcmp(externs[i].name, externName) == 0)
        {
            return externs[i].name;
        }
    }
    return NULL;
}

int isCorrectString(const char *name)
{
    if (name[0] == '"' && name[strlen(name) - 1] == '"')
    {
        return 1;
    }
    else
        return 0;
}
/* Check if a string is a number*/

int isNumber(const char *str)
{
    /* Check for empty string*/
    if (*str == '\0')
    {
        return 0;
    }

    /* Optional: Handle negative numbers*/
    if (*str == '-' || *str == '+')
    {
        str++;
    }

    while (*str != '\0')
    {
        if (!isdigit(*str))
        {
            return 0;
        }
        str++;
    }

    return 1;
}

/* Decimal to binary*/
char *decimalToBinary(int decimal, int length)
{
    char *binary = (char *)malloc(length + 1);
    if (!binary)
    {
        fprintf(stderr, "Error: Memory allocation failed.\n");
        exit(EXIT_FAILURE);
    }
    binary[length] = '\0';

    unsigned int mask = 1U << (length - 1);

    if (decimal < 0)
    {
        decimal = (1 << length) + decimal; /* Convert to two's complement*/
    }

    for (int i = 0; i < length; i++)
    {
        binary[i] = (decimal & mask) ? '1' : '0';
        mask >>= 1;
    }

    return binary;
}

/* binary to decimal*/
int binaryToDecimal(const char *binary)
{
    int decimal = 0;
    int length = strlen(binary);
    for (int i = 0; i < length; i++)
    {
        if (binary[length - 1 - i] == '1')
        {
            decimal += pow(2, i);
        }
    }
    return decimal;
}

/* Correct commas*/
int correctCommas(char *str)
{
    int expecting_object = 1; /* מצב שמחכה לאובייקט חדש*/
    while (*str)
    {
        if (*str == ',' && expecting_object)
        {
            /* פסיק מופיע מבלי שזוהה אובייקט קודם*/
            return 0;
        }
        else if (*str == ',' && !expecting_object)
        {
            /* מצבים תקינים, עוברים למצב שמחכה לאובייקט הבא*/
            expecting_object = 1;
        }
        else if (!isspace(*str))
        {
            /* זוהה אובייקט, מחכים לפסיק*/
            expecting_object = 0;
        }
        str++;
    }
    /* אם המחרוזת נגמרת במצב של ציפייה לאובייקט חדש, סימן שהיה פסיק מיותר בסוף*/
    return 1;
}
