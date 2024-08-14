#include <stdio.h>
#include <stdint.h>

// הגדרת אוגרים וזיכרון
int16_t registers[8] = {0};
int16_t memory[1024] = {0};

// קוד פעולות
enum {
    MOV = 0, CMP, ADD, SUB, LEA, CLR, NOT, INC, DEC, JMP, BNE, RED, PRN, JSR, RTS, STOP
};

// שיטות מיעון
enum {
    IMMEDIATE = 0,
    DIRECT,
    INDIRECT,
    REGISTER
};

// פונקציה לקידוד הוראת מכונה
void encode_instruction(uint16_t *code, uint8_t opcode, uint8_t src_mode, uint8_t src_reg, uint8_t dest_mode, uint8_t dest_reg) {
    code[0] = (opcode << 10) | (src_mode << 7) | (src_reg << 4) | (dest_mode << 1) | 1;
}

// פונקציה להדפסת בינארי
void print_binary(uint16_t value, int bits) {
    for (int i = bits - 1; i >= 0; i--) {
        putchar((value & (1 << i)) ? '1' : '0');
    }
    printf("\n");
}

int main() {
    uint16_t code[3];

    // דוגמה 1: mov #-1,r2
    encode_instruction(code, MOV, IMMEDIATE, 0, REGISTER, 2);
    code[1] = -1;  // מילת-מידע למיעון מיידי
    print_binary(code[0], 14);
    print_binary(code[1], 14);

    // דוגמה 2: dec x
    int16_t x = 23;
    encode_instruction(code, DEC, 0, 0, DIRECT, 0);
    code[1] = (uint16_t)&x;  // מילת-מידע למיעון ישיר
    print_binary(code[0], 14);
    print_binary(code[1], 14);

    // דוגמה 3: inc *r1
    encode_instruction(code, INC, INDIRECT, 1, 0, 0);
    print_binary(code[0], 14);

    // דוגמה 4: mov *r1, r2
    encode_instruction(code, MOV, INDIRECT, 1, REGISTER, 2);
    print_binary(code[0], 14);

    // דוגמה 5: clr r1
    encode_instruction(code, CLR, 0, 0, REGISTER, 1);
    print_binary(code[0], 14);

    return 0;
}