#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <math.h>

#define MAX_LINE_LENGTH 81
#define MAX_LABEL_NAME 32
#define CODE_SEGMENT_SIZE 15
#define CODE_SIZE 4
#define ARE_SIZE 3

// Constant strings for addressing methods
const char IMMEDIATE_ADDRESS[] = "0000";
const char DIRECT_ADDRESS[] = "0010";
const char INDIRECT_HOARD_ADDRESS[] = "0100";
const char DIRECT_HOARD_ADDRESS[] = "1000";
const char ABSOLUTE[] = "100";
const char RELOCATABLE[] = "010";
const char EXTERNAL[] = "001";
const char ZERO[] = "0000";

typedef struct {
    char symbol[MAX_LINE_LENGTH];
    char code[CODE_SEGMENT_SIZE];
    char externName[MAX_LABEL_NAME];
    int address;
    int isData;
    int isEntry;
    int isExtern;
} Symbol;

Symbol *symbols = NULL;
int symbolCount = 0;

typedef struct {
    char *name;
    int count;
} SymbolTable;

SymbolTable *symbolTable = NULL;
int labelCount = 0;

typedef struct {
    char *name;
} Extern;

Extern *exters = NULL;
int externCount = 0;

typedef struct {
    int wordCounter;
} L;

L *l = NULL;

int IC = 0;
int DC = 0;

// is register
int isRegister(const char *name) {
    const char *registerNum[] = {"mov", "cmp", "add", "sub", "lea", "clr", "not", "inc", "dec", "jmp", "bne", "red", "prn", "jsr", "rts", "stop"};
    int restrictedNamesCount = 16;
    if (name[0] == '*') {
        memmove(name, name + 1, strlen(name));
    }
    for (int i = 0; i < restrictedNamesCount; i++) {
        if (strcmp(name, registerNum[i]) == 0) {
            return i + 1;
        }
    }
    return 0;
}

// find method of addressing
char *findsMethodOfAddressing(const char *operand) {
    if (operand[0] == '#') { // a number
        int i = 1;
        while (operand[i] != '\0') {
            if (!isdigit(operand[i])) {
                return NULL;
            }
            i++;
        }
        return strdup(IMMEDIATE_ADDRESS);
    } else if (findLabbel(operand) != NULL) { // label
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

// miun operation
char *miunOperation(int num, const char *operand1, const char *operand2) {
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

// miun operand
char *miunOperand(const char *operand, int firstOperand) {
    static char code[CODE_SEGMENT_SIZE];
    char address[CODE_SIZE * 3];
    char addressingMethod[CODE_SIZE];
    char are[ARE_SIZE];

    if (operand[0] == '#') { // a number
        char num[strlen(operand) - 1 + 1];
        strncpy(num, &operand[1], strlen(operand) - 1);
        num[strlen(operand) - 1] = '\0';
        int value = atoi(num);
        strcpy(address, decimalToBinary(value, CODE_SIZE * 3));
        strcpy(are, ABSOLUTE);
    } else if (findLabbel(operand) != NULL) { // label
        Symbol *sym = findLabbel(operand);
        int value = symbols[sym->count].address;
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

// miun two registers
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
    
    symbols[symbolCount - 1].address = symbolCount + 100; // Not sure about the calculation, verify the correct address
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
    Extern *temp = realloc(exters, (externCount + 1) * sizeof(Extern));
    if (!temp) {
        fprintf(stderr, "Error: Memory allocation failed.\n");
        exit(EXIT_FAILURE);
    }
    exters = temp;
    externCount++;
    exters[externCount - 1].name = strdup(name);
    if (!exters[externCount - 1].name) {
        fprintf(stderr, "Error: Memory allocation failed.\n");
        exit(EXIT_FAILURE);
    }
}

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
Symbol *findLabbel(const char *symbolName) {
    for (int i = 0; i < symbolCount; i++) {
        if (strcmp(symbols[i].symbol, symbolName) == 0) {
            return &symbols[i];
        }
    }
    return NULL;
}

// Find external symbol
Extern *findExtern(const char *externName) {
    for (int i = 0; i < externCount; i++) {
        if (strcmp(exters[i].name, externName) == 0) {
            return &exters[i];
        }
    }
    return NULL;
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

// Decimal to octal
void decimalToOctal(int decimal, char* octal) {
    int i = 0;
    while (decimal > 0) {
        octal[i++] = (decimal % 8) + '0';
        decimal /= 8;
    }
    octal[i] = '\0';
   
    for (int j = 0; j < i / 2; j++) {
        char temp = octal[j];
        octal[j] = octal[i - j - 1];
        octal[i - j - 1] = temp;
    }
}

// Binary to octal
void binaryToOctal(const char* binary, char* octal) {
    int decimal = binaryToDecimal(binary);
    decimalToOctal(decimal, octal);
}

// First pass
void firstPass(FILE *sourceFile, SymbolTable *symbolTable, CodeSegment *codeSegment) {
    char line[MAX_LINE_LENGTH];
    char lineToCheck[MAX_LINE_LENGTH];
    int uncorrect = 0;
    int countLine = 0;
    char code[CODE_SEGMENT_SIZE];
    char newSymbolName[MAX_LABEL_NAME];
    int countWord = 0;

    while (fgets(line, MAX_LINE_LENGTH, sourceFile)) {
        countLine++;
        char *token = strtok(lineToCheck, " \t\n");
        if (token) {
            newSymbolName = labelDefinition(token); // Check if it is a new label
            if ((newSymbolName) != NULL) { // Label definition
                if (labelExist(newSymbolName)) { // Check if it exists
                    fprintf(stderr, "Error: line %d Symbol name exists.\n", countLine);
                    uncorrect = 1;
                } else if (restrictedNames(newSymbolName)) { // Check if it is a restricted name
                    fprintf(stderr, "Error: line %d Restricted name.\n", countLine);
                    uncorrect = 1;
                } else { // A new correct label
                    token = strtok(NULL, " \t\n"); // Next field
                }
            }
            if (token) {
                // Data
                if (strcmp(token, ".data") == 0) {
                    token = strtok(NULL, " \t\n");
                    char *num = strtok(token, ",");
                    int value = atoi(num); // Check if it is a number before?
                    strcpy(code, decimalToBinary(value, CODE_SEGMENT_SIZE));
                    addSymbol(newSymbolName, NULL, code, 1, 0, 0); // The first word
                    countWord++; // Add 1 L
                    int inComma = strlen(token) - strlen(num); // If no comma attached after = 0 else != 0
                    token = strtok(NULL, " \t\n");
                    while (token != NULL) { // Check numbers correctness
                        num = strtok(token, ",");
                        if (num && isNumber(num)) {
                            if (inComma == 1) { // After comma - correct
                                value = atoi(num);
                                strcpy(code, decimalToBinary(value, CODE_SEGMENT_SIZE)); // Create binary code
                                addSymbol(NULL, NULL, code, 1, 0, 0); // Add a symbol
                                countWord++;
                                inComma = strlen(token) - strlen(num);
                            } else if (inComma) { // Too much comma
                                fprintf(stderr, "Error: line %d Sequence of commas.\n", countLine);
                                uncorrect = 1;
                            } else { // Number not after comma
                                fprintf(stderr, "Error: line %d Comma is missing.\n", countLine);
                                uncorrect = 1;
                            }
                        } else if (num == NULL) { // Not a number but a comma??
                            if (inComma || strlen(token) > 1) { // Comma after comma
                                fprintf(stderr, "Error: line %d Sequence of commas.\n", countLine);
                                uncorrect = 1;
                            } else { // Not after comma
                                inComma = 1;
                            }
                        } else { // No digit, no comma
                            break;
                        }
                        token = strtok(NULL, " \t\n");
                    }

                    token = strtok(NULL, " \t\n");
                    if (token) {
                        fprintf(stderr, "Error: line %d Additional characters at the line.\n", countLine);
                        uncorrect = 1;
                    }
                    DC += countWord;
                // String
                } else if (strcmp(token, ".string") == 0) {
                    token = strtok(NULL, " \t\n");
                    int length = strlen(token);
                    char charToAdd = token[0];
                    int value = charToAdd; // ASCII value
                    strcpy(code, decimalToBinary(value, CODE_SEGMENT_SIZE)); // Create binary code
                    addSymbol(newSymbolName, NULL, code, 1, 0, 0); // The first word
                    countWord++;
                    for (int i = 1; i < length; i++) {
                        charToAdd = token[i];
                        value = charToAdd; // ASCII value
                        strcpy(code, decimalToBinary(value, CODE_SEGMENT_SIZE)); // Create binary code
                        addSymbol(NULL, code, 1, 0, 0); // All the next chars
                        countWord++;
                    }
                    strcpy(code, decimalToBinary(0, CODE_SEGMENT_SIZE)); // Create binary code
                    addSymbol(NULL, code, 1, 0, 0); // Add /0 at the end
                    countWord++;

                    token = strtok(NULL, " \t\n");
                    if (token) {
                        fprintf(stderr, "Error: line %d Additional characters at the line.\n", countLine);
                        uncorrect = 1;
                    }
                    DC += countWord;
                // Extern
                } else if (strcmp(token, ".extern") == 0) {
                    token = strtok(NULL, " \t\n");
                    while (token != NULL) {
                        addExtern(token); // Add extern
                        token = strtok(NULL, " \t\n");
                    }
                // Entry
                } else if (strcmp(token, ".entry") == 0) {
                    token = strtok(NULL, " \t\n");
                    Symbol *sym = findSymbol(symbolTable, token);
                    if (sym) {
                        sym->isEntry = 1;
                    }
                    token = strtok(NULL, " \t\n");
                    if (token) {
                        fprintf(stderr, "Error: line %d Additional characters at the line.\n", countLine);
                        uncorrect = 1;
                    }
                // Operation list // miun milca
                } else if (findOperation(token)) { // Ask miun milca
                    char operand1[] = NULL;
                    char operand2[] = NULL;
                    int oper = findOperation(token);
                    if (oper <= 5) { // The first group of instructions - receives 2 operands
                        countWord = 3; // 2 operands + operation = 3
                        token = strtok(NULL, " \t\n");
                        if (token) {
                            strcpy(operand1, token);
                        } else {
                            fprintf(stderr, "Error: Missing operand.\n");
                            uncorrect = 1;
                        }
                        token = strtok(NULL, " \t\n");
                        if (token) {
                            strcpy(operand2, token);
                        } else {
                            fprintf(stderr, "Error: line %d Missing operand.\n", countLine);
                            uncorrect = 1;
                        }

                        strcpy(code, miunOperation(oper, operand1, operand2)); // Create code to the operation
                        addSymbol(newSymbolName, NULL, code, 0, 0, 0); // Add operation to the symbols
                        if (isRegister(operand1) && isRegister(operand2)) {
                            strcpy(code, miunTwoRegister(operand1, operand2)); // Create 1 code to the registers
                            countWord--;
                        } else {
                            strcpy(code, miunOperand(operand1, 1)); // Create code to operand1
                            addSymbol(NULL, NULL, code, 0, 0, 0); // Add symbol with operand1

                            strcpy(code, miunOperand(operand2, 0)); // Create code to operand2
                            addSymbol(NULL, NULL, code, 0, 0, 0); // Add symbol with operand2
                        }
                    } else if (oper <= 14) { // The second group of instructions - receives 1 operand
                        countWord = 2; // Operand + operation = 2
                        token = strtok(NULL, " \t\n");
                        if (token) {
                            strcpy(operand1, token);
                        } else {
                            fprintf(stderr, "Error: line %d Missing operand.\n", countLine);
                            uncorrect = 1;
                        }

                        strcpy(code, miunOperation(oper, operand1, operand2)); // Create code to the operation
                        addSymbol(newSymbolName, NULL, code, 0, 0, 0); // Add operation to the symbols

                        strcpy(code, miunOperand(operand1, 1)); // Create code to operand1
                        addSymbol(NULL, NULL, code, 0, 0, 0); // Add symbol with operand1
                    } else { // The third group of instructions - doesn't receive any operands
                        countWord = 1; // Just the operation

                        strcpy(code, miunOperation(oper, operand1, operand2)); // Create code to the operation
                        addSymbol(newSymbolName, NULL, code, 0, 0, 0); // Add operation to the symbols
                    }
                    token = strtok(NULL, " \t\n");
                    if (token) {
                        fprintf(stderr, "Error: line %d Additional operands.\n", countLine);
                        uncorrect = 1;
                    }
                }
                IC += countWord;
            }
            // Uncorrect introduction
            else if (token != ';') {
                fprintf(stderr, "Error: line %d introduction? name incorrect.\n", countLine);
                uncorrect = 1;
            }
        }
        addL(countLine, countWord); // Updating L
        newSymbolName = NULL;
        countWord = 0;
    }	
    if (uncorrect == 1) {
        exit(EXIT_FAILURE);
    }
}

// Second pass
void secondPass(FILE *sourceFile, SymbolTable *symbolTable, CodeSegment *codeSegment) {
    char line[MAX_LINE_LENGTH];
    char lineToCheck[MAX_LINE_LENGTH];
    int uncorrect = 0;
    int countLine = 0;
    char code[CODE_SEGMENT_SIZE];
    char newSymbolName[MAX_LABEL_NAME];
    int countAdress = 1;
    fseek(sourceFile, 0, SEEK_SET); // Reset file pointer to the beginning
    while (fgets(line, MAX_LINE_LENGTH, sourceFile)) {
        strcpy(lineToCheck, line);
        countLine++;
        countAdress += l[countLine].wordCounter;
        char *token = strtok(lineToCheck, " \t\n");
        if (token) {
            newSymbolName = labelDefinition(token); // Check if it is a label
            if ((newSymbolName) != NULL) { // Label definition
                token = strtok(NULL, " \t\n"); // Next field
            }
        }
        if (token) {
            if (token == ';' || strcmp(token, ".data") == 0 || strcmp(token, ".string") == 0 || strcmp(token, ".extern") == 0) {
                continue;
            // Entry
            } else if (strcmp(token, ".entry") == 0) {
                token = strtok(NULL, " \t\n");
                Symbol *sym = findSymbol(symbolTable, token);
                if (sym) { // If found a label
                    if (sym->isEntry == 0) { // If not found in the first pass
                        sym->isEntry = 1;
                    }
                } else {  // Not exist in the table of labels
                    fprintf(stderr, "Error: line %d label not exist.\n", countLine);
                    uncorrect = 1;
                }
            // Operation list
            } else if (findOperation(token)) {
                token = strtok(NULL, " \t\n");
                while (token != NULL) {
                    if (symbols[countAdress + 1].code == NULL) { // Operand not found at the first pass
                        Symbol *sym = symbols[countAdress + 1];
                        SymbolTable *lab = findLabbel(token);
                        if (lab) { // It's a label
                            Symbol *sym = symbols[countAdress + 1];
                            are = "010";
                            address = decimalToBinary(sym.address, 12);
                            code = address + are;
                            strcpy(sym.code, code); // Updating code
                        }
                        else if (findExtern(token)) { // It's extern
                            strcpy(code, decimalToBinary(0, 12)); // Create code for external symbol
                            strcat(code, "001"); // Set ARE to "001" for external symbol
                            strcpy(symbols[countAdress + 1].code, code); // Update the symbol code
                        } else {
                            fprintf(stderr, "Error: line %d label not found.\n", countLine);
                            uncorrect = 1;
                        }
                    }
                    countAdress++;
                    token = strtok(NULL, " \t\n");
                }
            }
        }
    }

    if (uncorrect == 1) {
        exit(EXIT_FAILURE);
    }
}

// Build output files
void buildOutputFiles(const char *inputFilename) {
    // Create .ob file
    strcpy(objectFilename, inputFilename);
    strcat(objectFilename, ".ob");
    FILE *obFile = fopen(objectFilename, "w");
    // Create .ent file
    strcpy(entryFilename, inputFilename);
    strcat(entryFilename, ".ent");
    FILE *entFile = fopen(entryFilename, "w");
    // Create .ext file
    strcpy(externFilename, inputFilename);
    strcat(externFilename, ".ext");
    FILE *extFile = fopen(externFilename, "w");

    // Write to .ob file and to .ext file
    fprintf(obFile, "%d\t%d\n", IC, DC);
    for (int i = 0; i < symbolCount; i++) {
        Symbol *sym = symbols[i];
        int oct = binaryToOctal(sym->code);
        fprintf(obFile, "%04X\t%05X", sym->address, oct);
        if (sym->isExtern) {
            fprintf(extFile, "%s\t%04X", sym->externName, sym->address);
        }
    }

    // Write to .ent file
    for (int i = 0; i < labelCount; i++) {
        Symbol *sym = symbols[symbolTable[i].count];
        if (sym->isEntry) {
            fprintf(entFile, "%s\t%04X", symbolTable[i].name, sym->address);
        }
    }

    // No entries
    if (!entFile) {
        fclose(entFile);
        remove(entFile);
    } else {
        fclose(entFile);
    }
    // No externs
    if (!extFile) {
        fclose(extFile);
        remove(extFile);
    } else {
        fclose(extFile);
    }

    fclose(obFile);
}

// Main
int main(int argc, char *argv[]) {
    if (argc < 2) {
        printf("Usage: %s <source-file>\n", argv[0]);
        return 1;
    }

    FILE *sourceFile = fopen(argv[1], "r");
    if (!sourceFile) {
        perror("Failed to open source file");
        return 1;
    }

    SymbolTable symbolTable;
    CodeSegment codeSegment;

    initSymbolTable(&symbolTable);
    initCodeSegment(&codeSegment);

    // First pass
    firstPass(sourceFile, &symbolTable, &codeSegment);

    // Second pass
    secondPass(sourceFile, &symbolTable, &codeSegment);

    // Build output files
    buildOutputFiles(argv[1]);

    fclose(sourceFile);

    // Free allocated memory
    for (int i = 0; i < symbolCount; i++) {
        free(symbolTable[i].name);
    }
    free(symbolTable);
    for (int i = 0; i < externCount; i++) {
        free(exters[i].name);
    }
    free(exters);
    free(symbols);
    free(l);

    return 0;
}
// End of main