#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>

#define MAX_OPERANDS 2

typedef struct {
    char opcode[4];
    char operand1[20];
    int addressingMode1;
    char operand2[20];
    int addressingMode2;
} Instruction;

// Function to determine the addressing mode of an operand
int getAddressingMode(char *operand) {
    if (operand[0] == '#') {
        return 1;  // Immediate addressing
    } else if (operand[0] == '*' && (operand[1] == 'r' && isdigit(operand[2]) && operand[3] == '\0' && operand[2] >= '1' && operand[2] <= '8')) {
        return 3;  // Relative addressing (indirect register)
    } else if (operand[0] == 'r' && isdigit(operand[1]) && operand[2] == '\0' && operand[1] >= '1' && operand[1] <= '8') {
        return 4;  // Direct addressing (register)
    }
    return 2;  // Direct addressing (label or memory address)
}

// Function to validate the instruction and determine the addressing modes
int validateInstruction(char *line, Instruction *instr) {
    char *opcodes_with_two_operands[] = {"mov", "cmp", "add", "sub", "lea"};
    char *opcodes_with_one_operand[] = {"not", "clr", "inc", "dec", "jmp", "bne", "red", "prn", "jsr"};
    char *opcodes_with_no_operands[] = {"rts", "stop"};

    // Tokenize the input line
    char *token = strtok(line, " ,");
    if (token == NULL) return 0;

    // Copy the opcode
    strcpy(instr->opcode, token);

    // Check if the opcode is in the list of known opcodes
    int found = 0;
    for (int i = 0; i < sizeof(opcodes_with_two_operands) / sizeof(opcodes_with_two_operands[0]); i++) {
        if (strcmp(instr->opcode, opcodes_with_two_operands[i]) == 0) {
            found = 1;
            break;
        }
    }

    if (!found) {
        for (int i = 0; i < sizeof(opcodes_with_one_operand) / sizeof(opcodes_with_one_operand[0]); i++) {
            if (strcmp(instr->opcode, opcodes_with_one_operand[i]) == 0) {
                found = 1;
                break;
            }
        }
    }

    if (!found) {
        for (int i = 0; i < sizeof(opcodes_with_no_operands) / sizeof(opcodes_with_no_operands[0]); i++) {
            if (strcmp(instr->opcode, opcodes_with_no_operands[i]) == 0) {
                found = 1;
                break;
            }
        }
    }

    if (!found) return 0;

    // Parse operands based on opcode
    if (instr->opcode[0] != '\0') {
        if (strcmp(instr->opcode, "rts") == 0 || strcmp(instr->opcode, "stop") == 0) {
            return 1; // No operands expected
        }

        token = strtok(NULL, " ,");
        if (token != NULL) {
            strcpy(instr->operand1, token);
            instr->addressingMode1 = getAddressingMode(instr->operand1);
        } else if (strcmp(instr->opcode, "rts") != 0 && strcmp(instr->opcode, "stop") != 0) {
            return 0; // Missing operands
        }

        token = strtok(NULL, " ,");
        if (token != NULL) {
            if (instr->operand1[0] == '\0') return 0; // Invalid operand
            strcpy(instr->operand2, token);
            instr->addressingMode2 = getAddressingMode(instr->operand2);
        }

        token = strtok(NULL, " ,");
        if (token != NULL) {
            return 0; // Too many operands
        }
    }

    return 1; // Instruction is valid
}

int main() {
    char line[100];
    Instruction instr = {"", "", 0, "", 0};

    printf("Enter an instruction: ");
    fgets(line, sizeof(line), stdin);

    // Remove trailing newline character
    line[strcspn(line, "\n")] = 0;

    if (validateInstruction(line, &instr)) {
        printf("Valid instruction\n");
        printf("Opcode: %s\n", instr.opcode);
        if (instr.operand1[0] != '\0') printf("Operand 1: %s, Addressing Mode: %d\n", instr.operand1, instr.addressingMode1);
        if (instr.operand2[0] != '\0') printf("Operand 2: %s, Addressing Mode: %d\n", instr.operand2, instr.addressingMode2);
    } else {
        printf("Invalid instruction\n");
    }

    return 0;
}
