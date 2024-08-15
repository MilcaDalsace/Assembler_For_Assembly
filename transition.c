
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <math.h>
#include <ctype.h>
#include "function.h"
#include "transitions.h"
// First pass
void firstPass(const char *sourceFilename) {
	FILE *sourceFile = fopen(sourceFilename, "r");
	if (!sourceFile) {
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

    while (fgets(line, MAX_LINE_LENGTH, sourceFile)) {
        countLine++;
	strcpy(lineToCheck,line);
        char *token = strtok(lineToCheck, " \t\n");
        if (token) {
            newSymbolName = labelDefinition(token); // Check if it is a new label
            if ((newSymbolName) != NULL) { // Label definition
                if (findLabbel(newSymbolName) +1) { // Check if it exists
                    fprintf(stderr, "Error: line %d Symbol name exists.\n", countLine);
                    uncorrect = 1;
                } else if (isNameRestricted(newSymbolName)) { // Check if it is a restricted name
                    fprintf(stderr, "Error: line %d %s  is restricted name.\n", countLine,newSymbolName);
                    uncorrect = 1;
                } else { // A new correct label
                    token = strtok(NULL, " \t\n"); // Next field
		    fprintf(stderr, "line: %d %s naw symbol name.\n",countLine, newSymbolName);//check	
                }
            }
            if (token) {
                // Data
                if (strcmp(token, ".data") == 0) {
		char *remaining = strtok(NULL, ""); // Get the rest of the lin

                    if (remaining != NULL&&correctCommas(remaining)) {
			fprintf(stderr, " line %d is data.\n", countLine);
                        while (token != NULL) {
                            token = strtok(NULL, ", \t\n");
                            if (isNumber(token)) {
                                int value = atoi(token);
                                strcpy(code, decimalToBinary(value, CODE_SEGMENT_SIZE));
                                addSymbol(newSymbolName, NULL, code, 1, 0, 0); // the first word
                                countWord++; // add 1 L
                            } else {
                                fprintf(stderr, "Error: line %d not a number \n", countLine);
                                uncorrect = 1;
                            }
                        }
		    } else {
 			fprintf(stderr, "Error: line %d \n", countLine);
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
                        addSymbol(NULL, NULL, code, 1, 0, 0); // All the next chars
                        countWord++;
                    }
                    strcpy(code, decimalToBinary(0, CODE_SEGMENT_SIZE)); // Create binary code
                    addSymbol(NULL, NULL, code, 1, 0, 0); // Add /0 at the end
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
			fprintf(stderr, " line: %d %s add extern.\n",countLine, externs[0].name);
                        token = strtok(NULL, " \t\n");
                    }
                // Entry
                } else if (strcmp(token, ".entry") == 0) {
                    token = strtok(NULL, " \t\n");
		    if(token){
			    if (findLabbel(token)+1){
		               Symbol *sym = &symbols[findLabbel(token)];
		               if (sym) {
		                  sym->isEntry = 1;
				fprintf(stderr, " line: %d %s entry andicated.\n", countLine,token);
		               }
			    }else{
				fprintf(stderr, " line: %d %s not yet found in labbels.\n", countLine,token);
		               }
			    
		               token = strtok(NULL, " \t\n");
		               if (token) {
		                  fprintf(stderr, "Error: line %d Additional characters at the line.\n", countLine);
		                  uncorrect = 1;
		               }
		    }else{
			fprintf(stderr, "Error: line %d Labbel name is missing.\n", countLine);
		                  uncorrect = 1;
		    }
                // Operation list 
			
                } else if (findOperation(token)+1) {
		    char *remaining = strtok(NULL,"");
			
			
                    if (remaining!=NULL&&correctCommas(remaining)) {
                        char operand1[MAX_LINE_LENGTH];
                        char operand2[MAX_LINE_LENGTH];
                        int oper = findOperation(token)+1;
                        if (oper <= 5) { // The first group of instructions - receives 2 operands
			
                            countWord = 3; // 2 operands + operation = 3
                            token = strtok(remaining, ",\0\t\n");
				fprintf(stderr, " line: %d %s entry andicated.\n", countLine, token);
                            if (token) {	
                               strcpy(operand1, token);
				fprintf(stderr, " line: %d %s is operand1.\n",countLine, operand1);//check
				token = strtok(NULL, ",\0\t\n");
                            } else {
                                fprintf(stderr, "Error: line %d Missing operand1.\n",countLine);
                                uncorrect = 1;
                            }
				fprintf(stderr, " line: %d %s is operand 2.\n",countLine, token);//check
                            if (token) {
                                strcpy(operand2, token);
				fprintf(stderr, " line %d %s is operand2 add.\n", countLine,operand2);//check
                            } else {
                                fprintf(stderr, "Error: line %d Missing operand2.\n", countLine);
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
                    
 			} else {
			 fprintf(stderr, "Error: line %d.\n", countLine);
			fprintf(stderr, "Error: po line %d.\n", countLine);
                        uncorrect = 1;
                   }
                    IC += countWord;
                }
           
            // Uncorrect introduction
            }else if (strcmp(token,";")) {
                fprintf(stderr, "Error: line %d introduction? name incorrect.\n", countLine);
                uncorrect = 1;
            }
        }
        addL(countLine, countWord); // Updating L
        newSymbolName = NULL;
        countWord = 0;
    }	
	 fclose(sourceFile);
   // if (uncorrect == 1) {
      //  exit(EXIT_FAILURE);
  //  }
}

// Second pass
void secondPass(const char *sourceFilename) {

    FILE *sourceFile = fopen(sourceFilename, "r");
    if (!sourceFile) {
        perror("Error opening input file");
        return;
    }
    char line[MAX_LINE_LENGTH];
    char lineToCheck[MAX_LINE_LENGTH];
    int uncorrect = 0;
    int countLine = 0;
    char code[CODE_SEGMENT_SIZE];
    char *newSymbolName=NULL;
    int countAdress = 1;
	fprintf(stderr, "second past %d.\n", countLine);
    //fseek(sourceFile, 0, SEEK_SET); // Reset file pointer to the beginning
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
            if (strcmp(token,";")|| strcmp(token, ".data") == 0 || strcmp(token, ".string") == 0 || strcmp(token, ".extern") == 0) {
                continue;
            // Entry
            } else if (strcmp(token, ".entry") == 0) {
                token = strtok(NULL, " \t\n");
		if (findLabbel(token) != -1){
		    Symbol *sym = &symbols[findLabbel(token)];
                    if (sym) { // If found a label
                        if (sym->isEntry == 0) { // If not found in the first pass
                            sym->isEntry = 1;
                        }
                   } else {  // Not exist in the table of labels
                    fprintf(stderr, "Error: line %d label not exist.\n", countLine);
                    uncorrect = 1;
                   }
		}else{
			fprintf(stderr, "Error: line %d Labbel name is missing.\n", countLine);
		                  uncorrect = 1;
		}

            // Operation list
            } else if (findOperation(token)) {
                token = strtok(NULL, " \t\n");
                while (token != NULL) {
                    if (symbols[countAdress + 1].code == NULL) { // Operand not found at the first pass
                        if (findLabbel(token) != -1) { // It's a label
                            Symbol *sym = &symbols[countAdress + 1];
                            char *are = "010";
                            char *address = decimalToBinary(sym->address, 12);
			    char code[CODE_SEGMENT_SIZE + ARE_SIZE + 1]; // +1 בשביל תו הסיום '\0'
			    snprintf(code, sizeof(code), "%s%s", address, are);
                            strcpy(sym->code, code); // Updating code
                        } else if (findExtern(token)) { // It's extern
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
    char objectFilename[MAX_LINE_LENGTH];
    strcpy(objectFilename, inputFilename);
    strcat(objectFilename, ".ob");
    FILE *obFile = fopen(objectFilename, "w");

    // Create .ent file
    char entryFilename[MAX_LINE_LENGTH];
    strcpy(entryFilename, inputFilename);
    strcat(entryFilename, ".ent");
    FILE *entFile = fopen(entryFilename, "w");

    // Create .ext file
    char externFilename[MAX_LINE_LENGTH];
    strcpy(externFilename, inputFilename);
    strcat(externFilename, ".ext");
    FILE *extFile = fopen(externFilename, "w");

    // Write to .ob file and to .ext file
    fprintf(obFile, "%d\t%d\n", IC, DC);
    for (int i = 0; i < symbolCount; i++) {
        Symbol *sym = &symbols[i];
        char octal[CODE_SEGMENT_SIZE];
        binaryToOctal(sym->code, octal);
        fprintf(obFile, "%04X\t%s", sym->address, octal);
        if (sym->isExtern) {
            fprintf(extFile, "%s\t%04X\n", sym->externName, sym->address);
        }
    }

    // Write to .ent file
    for (int i = 0; i < labelCount; i++) {
        Symbol *sym = &symbols[symbolTable[i].count];
        if (sym->isEntry) {
            fprintf(entFile, "%s\t%04X\n", symbolTable[i].name, sym->address);
        }
    }

    // Close files
    if (entFile) fclose(entFile);
    if (extFile) fclose(extFile);
    fclose(obFile);
}
