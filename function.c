#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <math.h>
#include <ctype.h>
#include "function.h"

// Constant strings for addressing methods
const char IMMEDIATE_ADDRESS[] = "0000";
const char DIRECT_ADDRESS[] = "0010";
const char INDIRECT_HOARD_ADDRESS[] = "0100";
const char DIRECT_HOARD_ADDRESS[] = "1000";
const char ABSOLUTE[] = "100";
const char RELOCATABLE[] = "010";
const char EXTERNAL[] = "001";
const char ZERO[] = "0000";

Symbol *symbols = NULL;
int symbolCount = 0;

SymbolTable *symbolTable = NULL;
int labelCount = 0;

Extern *externs = NULL;
int externCount = 0;

L *l = NULL;

int IC = 0;
int DC = 0;

// Restricted name
int isNameRestricted(const char *name) {
    if (isRegister(name)+1) {
        return 1;
    }else if(findOperation(name)+1){
	return 1;
    }
    // Add macro checking
    return 0;
}

//find operation

int findOperation(const char *name){
 const char *operations[] = {"mov", "cmp", "add", "sub", "lea", "clr", "not", "inc", "dec", "jmp", "bne", "red", "prn", "jsr", "rts", "stop"};
  int restrictedNamesCount = 16;
 for (int i = 0; i < restrictedNamesCount; i++) {
        if (strcmp(name, operations[i]) == 0) {
            return i ;
        }
  }
 return -1;
}

// Is register
int isRegister(const char *name) {
const char *registers[] = {"r0", "r1", "r2", "r3", "r4", "r5", "r6", "r7"};
int registerNum = 8;
    if (name[0] == '*') {
        memmove((char *)name, name + 1, strlen(name));
    }
    for (int i = 0; i < registerNum; i++) {
        if (strcmp(name, registers[i]) == 0) {
            return i;
        }
    }
    return -1;
}

// Find method of addressing
char* findsMethodOfAddressing(const char *operand) {
    if (operand[0] == '#') { // a number
        int i = 1;
        while (operand[i] != '\0') {
            if (!isdigit(operand[i])) {
                return NULL;
            }
            i++;
        }
        return strdup(IMMEDIATE_ADDRESS);
    } else if (findLabbel(operand) != -1) { // label
        return strdup(DIRECT_ADDRESS);
    } else if (findExtern(operand) != NULL) { // external label
        return strdup(DIRECT_ADDRESS);
    } else if (operand[0] == '*') { // A pointer to a register
        char reg[strlen(operand) - 1 + 1];
        strncpy(reg, &operand[1], strlen(operand) - 1);
        reg[strlen(operand) - 1] = '\0';
        if (isRegister(reg)) {
            return strdup(INDIRECT_HOARD_ADDRESS);
        } else {
            return NULL;
        }
    } else if (isRegister(operand)) { // is register
        return strdup(DIRECT_HOARD_ADDRESS);
    } else {
        return NULL;
    }
}

// Miun operation
char* miunOperation(int num, const char *operand1, const char *operand2) {
    static char code[CODE_SEGMENT_SIZE];
    char opCode[CODE_SIZE];
    char addressingMethod1[CODE_SIZE];
    char addressingMethod2[CODE_SIZE];
    char are[ARE_SIZE];

    char *method = NULL;

    strcpy(opCode, decimalToBinary(num - 1, CODE_SIZE));
    strcpy(are, ABSOLUTE);

    if (operand1 == NULL) { // no operands-third group
        strcpy(addressingMethod1, "0000");
        strcpy(addressingMethod2, "0000");
    } else {
        method = findsMethodOfAddressing(operand1);
        if (method == NULL) {
            return NULL;
        } else {
            strncpy(addressingMethod1, method, CODE_SIZE);
            free(method);
        }

        if (operand2 == NULL) { // own operand - second group
            strcpy(addressingMethod2, "0000");
        } else { // two operands - first group
            method = findsMethodOfAddressing(operand2);
            if (method == NULL) {
                return NULL;
            } else {
                strncpy(addressingMethod2, method, CODE_SIZE);
                free(method);
            }
        }
    }
    snprintf(code, CODE_SEGMENT_SIZE, "%s%s%s%s", opCode, addressingMethod1, addressingMethod2, are);
    return code;
}

// Miun operand
char *miunOperand(const char *operand, int firstOperand) {
    static char code[CODE_SEGMENT_SIZE];
    char address[CODE_EE_SIZE];
    char addressingMethod[CODE_SIZE];
    char are[ARE_SIZE];

    if (operand[0] == '#') { // a number
        char num[strlen(operand) - 1 + 1];
        strncpy(num, &operand[1], strlen(operand) - 1);
        num[strlen(operand) - 1] = '\0';
        int value = atoi(num);
        strcpy(address, decimalToBinary(value, CODE_SIZE * 3));
        strcpy(are, ABSOLUTE);
    } else if (findLabbel(operand) != -1) { // label
        Symbol *sym = &symbols[findLabbel(operand)];
        int value = sym->address;
        strcpy(address, decimalToBinary(value, CODE_SIZE * 3));
        strcpy(are, RELOCATABLE);
    } else if (findExtern(operand) != NULL) { // external label
        strcpy(address, decimalToBinary(0, CODE_SIZE * 3));
        strcpy(are, EXTERNAL);
    } else if (isRegister(operand)) { // A pointer to a register
        int num = isRegister(operand);
        strcpy(addressingMethod, decimalToBinary(num - 1, CODE_SIZE));
        if (firstOperand) {
            snprintf(address, sizeof(address), "%s%s%s", ZERO, addressingMethod, ZERO);
        } else {
            snprintf(address, sizeof(address), "%s%s%s", ZERO, ZERO, addressingMethod);
        }
        strcpy(are, ABSOLUTE);
    } else { // not correct
        return NULL;
    }
    snprintf(code, CODE_SEGMENT_SIZE, "%s%s", address, are);
    return code;
}

// Miun two registers
char *miunTwoRegister(const char *operand1, const char *operand2) {
    static char code[CODE_SEGMENT_SIZE];
    char addressingMethod1[CODE_SIZE];
    char addressingMethod2[CODE_SIZE];

    int num = isRegister(operand1);
    strcpy(addressingMethod1, decimalToBinary(num - 1, CODE_SIZE));
    num = isRegister(operand2);
    strcpy(addressingMethod2, decimalToBinary(num - 1, CODE_SIZE));
    
    snprintf(code, CODE_SEGMENT_SIZE, "%s%s%s%s", ZERO, addressingMethod1, addressingMethod2, ABSOLUTE);
    return code;
}

// Add symbol
void addSymbol(const char *symbolName, const char *externName, const char *code, const int isData, const int isEntry, const int isExtern) {
    Symbol *temp = realloc(symbols, (symbolCount + 1) * sizeof(Symbol));
    if (!temp) {
        fprintf(stderr, "Error: Memory allocation failed.\n");
        exit(EXIT_FAILURE);
    }
    symbols = temp;
    symbolCount++;
    
    if (symbolName != NULL) {
        strncpy(symbols[symbolCount - 1].symbol, symbolName, MAX_LINE_LENGTH - 1);
        symbols[symbolCount - 1].symbol[MAX_LINE_LENGTH - 1] = '\0';
    } else {
        symbols[symbolCount - 1].symbol[0] = '\0';
    }
    
    if (externName != NULL) {
        strncpy(symbols[symbolCount - 1].externName, externName, MAX_LABEL_NAME - 1);
        symbols[symbolCount - 1].externName[MAX_LABEL_NAME - 1] = '\0';
    } else {
        symbols[symbolCount - 1].externName[0] = '\0';
    }
    
    strncpy(symbols[symbolCount - 1].code, code, CODE_SEGMENT_SIZE - 1);
    symbols[symbolCount - 1].code[CODE_SEGMENT_SIZE - 1] = '\0';
    
    symbols[symbolCount - 1].address = symbolCount + 99; // Not sure about the calculation, verify the correct address
    symbols[symbolCount - 1].isData = isData;
    symbols[symbolCount - 1].isEntry = isEntry;
    symbols[symbolCount - 1].isExtern = isExtern;

    if (symbolName != NULL) { // Add label if present
        addLabbel(symbolName, symbolCount - 1);
    }
}

// Add label to symbol table
void addLabbel(const char *name, const int count) {
    SymbolTable *temp = realloc(symbolTable, (labelCount + 1) * sizeof(SymbolTable));
    if (!temp) {
        fprintf(stderr, "Error: Memory allocation failed.\n");
        exit(EXIT_FAILURE);
    }
    symbolTable = temp;
    labelCount++;
    symbolTable[labelCount - 1].name = strdup(name);
    if (!symbolTable[labelCount - 1].name) {
        fprintf(stderr, "Error: Memory allocation failed.\n");
        exit(EXIT_FAILURE);
    }
    symbolTable[labelCount - 1].count = count;
}

// Add external symbol
void addExtern(const char *name) {
    Extern *temp = realloc(externs, (externCount + 1) * sizeof(Extern));
    if (!temp) {
        fprintf(stderr, "Error: Memory allocation failed.\n");
        exit(EXIT_FAILURE);
    }
    externs = temp;
    externCount++;
    externs[externCount - 1].name = strdup(name);
    if (!externs[externCount - 1].name) {
        fprintf(stderr, "Error: Memory allocation failed.\n");
        exit(EXIT_FAILURE);
    }
}

// Add line count
void addL(int counteLine, int wordCounter) {
    L *temp = realloc(l, counteLine * sizeof(L));
    if (!temp) {
        fprintf(stderr, "Error: Memory allocation failed.\n");
        exit(EXIT_FAILURE);
    }
    l = temp;
    l[counteLine - 1].wordCounter = wordCounter;
}

// Label definition
char *labelDefinition(const char *token) {
    int length = strlen(token);
    if (token[length - 1] == ':') {
        char *newSymbolName = malloc(length);
        if (!newSymbolName) {
            fprintf(stderr, "Error: Memory allocation failed.\n");
            exit(EXIT_FAILURE);
        }
        strncpy(newSymbolName, token, length - 1); // Copy the name without ':'
        newSymbolName[length - 1] = '\0';
        return newSymbolName;
    }
    return NULL;
}

// Find label
int findLabbel(const char *symbolName) {
    for (int i = 0; i < symbolCount; i++) {
        if (strcmp(symbols[i].symbol, symbolName) == 0) {
            return i;
        }
    }
    return -1;
}

// Find external symbol
Extern* findExtern(const char *externName) {
    for (int i = 0; i < externCount; i++) {
        if (strcmp(externs[i].name, externName) == 0) {
            return &externs[i];
        }
    }
    return NULL;
}

// Check if a string is a number
int isNumber(const char *str) {
    // Check for empty string
    if (*str == '\0') {
        return 0;
    }

    // Optional: Handle negative numbers
    if (*str == '-' || *str == '+') {
        str++;
    }

    while (*str != '\0') {
        if (!isdigit(*str)) {
            return 0;
        }
        str++;
    }

    return 1;
}

// Decimal to binary
char *decimalToBinary(int decimal, int length) {
    char *binary = (char *)malloc(length + 1);
    if (!binary) {
        fprintf(stderr, "Error: Memory allocation failed.\n");
        exit(EXIT_FAILURE);
    }
    binary[length] = '\0';
    for (int i = length - 1; i >= 0; i--) {
        binary[i] = (decimal % 2) ? '1' : '0';
        decimal /= 2;
    }
    return binary;
}

//binary to decimal
int binaryToDecimal(const char* binary) {
    int decimal = 0;
    int length = strlen(binary);
    for (int i = 0; i < length; i++) {
        if (binary[length - 1 - i] == '1') {
            decimal += pow(2, i);
        }
    }
    return decimal;
}



// Correct commas
int correctCommas(char *str) {
    int expecting_object = 1; // מצב שמחכה לאובייקט חדש
    while (*str) {
        if (*str == ',' && expecting_object) {
            // פסיק מופיע מבלי שזוהה אובייקט קודם
            return 0;
        } else if (*str == ',' && !expecting_object) {
            // מצבים תקינים, עוברים למצב שמחכה לאובייקט הבא
            expecting_object = 1;
        } else if (!isspace(*str)) {
            // זוהה אובייקט, מחכים לפסיק
            expecting_object = 0;
        }
        str++;
    }
    // אם המחרוזת נגמרת במצב של ציפייה לאובייקט חדש, סימן שהיה פסיק מיותר בסוף
    return !expecting_object;
}


