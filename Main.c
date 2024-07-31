#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#define MAX_LINE_LENGTH 100
#define SYMBOL_TABLE_SIZE 100
#define CODE_SEGMENT_SIZE 1000

typedef struct {
    char symbol[MAX_LINE_LENGTH];
    int address;
    int isData;
    int isEntry;
    int isExtern;
} Symbol;

typedef struct {
    Symbol table[SYMBOL_TABLE_SIZE];
    int count;
} SymbolTable;

typedef struct {
    char code[CODE_SEGMENT_SIZE];
    int IC;
    int DC;
} CodeSegment;

void initCodeSegment(CodeSegment *cs) {
    cs->IC = 0;
    cs->DC = 0;
}

void initSymbolTable(SymbolTable *st) {
    st->count = 0;
}

void addSymbol(SymbolTable *st, char *symbol, int address, int isData, int isEntry, int isExtern) {
    strcpy(st->table[st->count].symbol, symbol);
    st->table[st->count].address = address;
    st->table[st->count].isData = isData;
    st->table[st->count].isEntry = isEntry;
    st->table[st->count].isExtern = isExtern;
    st->count++;
}

Symbol* findSymbol(SymbolTable *st, char *symbol) {
    for (int i = 0; i < st->count; i++) {
        if (strcmp(st->table[i].symbol, symbol) == 0) {
            return &st->table[i];
        }
    }
    return NULL;
}

void firstPass(FILE *sourceFile, SymbolTable *symbolTable, CodeSegment *codeSegment) {
    char line[MAX_LINE_LENGTH];
    while (fgets(line, MAX_LINE_LENGTH, sourceFile)) {
        // Process line
        // Example: if line starts with "data" or "string" or "extern" or "entry" etc.
        // Update symbol table and code segment accordingly
        // This is a simplified example
        if (strstr(line, "data") || strstr(line, "string") || strstr(line, "extern")) {
            // Skip to next line
            continue;
        } else if (strstr(line, "entry")) {
            // Mark symbol as entry
            // Example: entry SYMBOL
            char symbol[MAX_LINE_LENGTH];
            sscanf(line, "entry %s", symbol);
            Symbol *sym = findSymbol(symbolTable, symbol);
            if (sym) {
                sym->isEntry = 1;
            }
        } else {
            // Process instruction
            // Example: SYMBOL: INSTRUCTION OPERANDS
            char symbol[MAX_LINE_LENGTH];
            if (sscanf(line, "%s:", symbol) == 1) {
                // Add symbol to symbol table with current IC
                addSymbol(symbolTable, symbol, codeSegment->IC, 0, 0, 0);
            }
            codeSegment->IC += 4; // Assuming each instruction is 4 bytes
        }
    }
}

void secondPass(FILE *sourceFile, SymbolTable *symbolTable, CodeSegment *codeSegment) {
    char line[MAX_LINE_LENGTH];
    fseek(sourceFile, 0, SEEK_SET); // Reset file pointer to the beginning
    while (fgets(line, MAX_LINE_LENGTH, sourceFile)) {
        // Process line
        // Example: if line starts with "data" or "string" or "extern" or "entry" etc.
        // Update symbol table and code segment accordingly
        if (strstr(line, "data") || strstr(line, "string") || strstr(line, "extern")) {
            // Skip to next line
            continue;
        } else if (strstr(line, "entry")) {
            // Skip to next line
            continue;
        } else {
            // Process instruction
            // Example: SYMBOL: INSTRUCTION OPERANDS
            char symbol[MAX_LINE_LENGTH];
            if (sscanf(line, "%s:", symbol) == 1) {
                // Process instruction
                // This is a simplified example
            }
        }
    }
}

void buildOutputFiles(SymbolTable *symbolTable, CodeSegment *codeSegment) {
    // Generate output files based on the symbol table and code segment
    // Example: write to .ob, .ent, .ext files
    FILE *obFile = fopen("output.ob", "w");
    FILE *entFile = fopen("output.ent", "w");
    FILE *extFile = fopen("output.ext", "w");

    // Write to .ob file
    fprintf(obFile, "Code Segment:\n");
    for (int i = 0; i < codeSegment->IC; i++) {
        fprintf(obFile, "%02X ", codeSegment->code[i] & 0xFF);
        if ((i + 1) % 16 == 0) {
            fprintf(obFile, "\n");
        }
    }
    fprintf(obFile, "\n");

    // Write to .ent and .ext files
    for (int i = 0; i < symbolTable->count; i++) {
        Symbol *sym = &symbolTable->table[i];
        if (sym->isEntry) {
            fprintf(entFile, "%s %04X\n", sym->symbol, sym->address);
        }
        if (sym->isExtern) {
            fprintf(extFile, "%s %04X\n", sym->symbol, sym->address);
        }
    }

    fclose(obFile);
    fclose(entFile);
    fclose(extFile);
}

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
    buildOutputFiles(&symbolTable, &codeSegment);

    fclose(sourceFile);
    return 0;
}
