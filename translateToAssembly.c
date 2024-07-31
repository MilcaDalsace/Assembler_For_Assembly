#include <stdio.h>
#include <stdint.h>

// פונקציה להמיר ערכים לאסמבלי
void translateToAssembly(uint8_t opcode, uint8_t srcMode, uint8_t destMode, uint8_t E, uint8_t R, uint8_t A) {
    // בדוק אם הערכים נכונים
    if (opcode > 15 || srcMode > 3 || destMode > 3) {
        printf("Error: Invalid values for opcode or addressing modes.\n");
        return;
    }

    // הדפס את שם הפעולה בהתאם לopcode
    const char* operations[16] = {
        "mov", "cmp", "add", "sub", "lea", "clr", "not", "inc",
        "dec", "jmp", "bne", "red", "prn", "jsr", "rts", "stop"
    };

    printf("Opcode: %s\n", operations[opcode]);

    // הדפס את שיטת המיעון
    const char* addressingModes[4] = {
        "Direct", "Indirect", "Register", "Indexed"
    };

    printf("Source Addressing Mode: %s\n", addressingModes[srcMode]);
    printf("Destination Addressing Mode: %s\n", addressingModes[destMode]);

    // הדפס את ערכי E, R, A
    printf("E: %d, R: %d, A: %d\n", E, R, A);
}

int main() {
    // דוגמה לשימוש בפונקציה
    uint8_t opcode = 2;   // למשל חיבור (add)
    uint8_t srcMode = 1;  // שיטת מיעון מקור עקיפה
    uint8_t destMode = 2; // שיטת מיעון יעד אינדירקטיבית
    uint8_t E = 0;
    uint8_t R = 0;
    uint8_t A = 1;       // ערך A תמיד 1

    // קרא לפונקציה
    translateToAssembly(opcode, srcMode, destMode, E, R, A);

    return 0;
}
