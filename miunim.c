#include <stdio.h>
#include <stdint.h>

// פונקציה להמיר ערך לשיטת המשלים ל-2 עם רוחב של 12 סיביות
uint16_t toTwoComplement(int16_t value) {
    if (value < 0) {
        return (uint16_t)(0xFFF & (uint16_t)value); // לוקח את 12 הסיביות האחרונות
    } else {
        return (uint16_t)value;
    }
}

// פונקציה לכתיבת הוראת אסמבלי עם מיעון מיידי
void assembleImmediate(uint16_t immediateValue, uint8_t registerNumber) {
    const uint8_t opcode = 0;  // נניח שהopcode של mov הוא 0
    const uint8_t srcMode = 0; // Immediate addressing mode
    const uint8_t destMode = 3; // Register addressing mode (עבור אוגר)

    // המרת הערך לשיטת המשלים ל-2
    uint16_t immediateBinary = toTwoComplement(immediateValue);

    // קידוד המילה הראשונה
    uint16_t word1 = (opcode << 11) | (srcMode << 7) | (destMode << 3) | 0x4; // A=1, E=R=0
    printf("First Word: 0x%04X\n", word1);

    // הדפסת הערך הבינארי של האופרנד
    printf("Second Word: 0x%03X\n", immediateBinary);
}

// פונקציה לכתיבת הוראת אסמבלי עם מיעון ישיר
void assembleDirect(uint16_t address, uint8_t isExternal) {
    const uint8_t opcode = 1; // נניח שהopcode של `dec` הוא 1
    const uint8_t srcMode = 1; // Direct addressing mode
    const uint8_t destMode = 0; // No destination mode (בהנחה שאין יעד)

    // קידוד המילה הראשונה
    uint16_t word1 = (opcode << 11) | (srcMode << 7) | (destMode << 3) | 0x4; // A=1, E=R=0
    printf("First Word: 0x%04X\n", word1);

    // אם הכתובת חיצונית, קודד את E ל-1
    uint16_t word2 = (isExternal ? 0x8000 : 0x0000) | (address & 0x0FFF); // כתובת 12 סיביות
    printf("Second Word: 0x%04X\n", word2);
}

// פונקציה לכתיבת הוראת אסמבלי עם מיעון אוגר עקיף
void assembleIndirect(uint8_t regNumber) {
    const uint8_t opcode = 2; // נניח שהopcode של `inc` הוא 2
    const uint8_t srcMode = 2; // Indirect addressing mode
    const uint8_t destMode = 0; // No destination mode

    // קידוד המילה הראשונה
    uint16_t word1 = (opcode << 11) | (srcMode << 7) | (destMode << 3) | 0x4; // A=1, E=R=0
    printf("First Word: 0x%04X\n", word1);

    // קידוד האוגר
    uint16_t word2 = (regNumber << 6) & 0x003F; // קודד את מספר האוגר
    printf("Second Word: 0x%04X\n", word2);
}

// פונקציה לכתיבת הוראת אסמבלי עם מיעון אוגר ישיר
void assembleRegisterDirect(uint8_t regNumber) {
    const uint8_t opcode = 3; // נניח שהopcode של `clr` הוא 3
    const uint8_t srcMode = 0; // Register addressing mode
    const uint8_t destMode = 0; // Register addressing mode

    // קידוד המילה הראשונה
    uint16_t word1 = (opcode << 11) | (srcMode << 7) | (destMode << 3) | 0x4; // A=1, E=R=0
    printf("First Word: 0x%04X\n", word1);

    // קידוד האוגר
    uint16_t word2 = (regNumber << 6) & 0x003F; // קודד את מספר האוגר
    printf("Second Word: 0x%04X\n", word2);
}

int main() {
    uint8_t regNumber = 1; // לדוגמה אוגר 1
    uint16_t address = 22; // לדוגמה
    uint8_t isExternal = 0; // פנימית
    int16_t immediateValue = -1; // לדוגמה
    uint8_t registerNumber = 2;  // אוגר 2

    assembleImmediate(immediateValue, registerNumber);
    assembleDirect(address, isExternal);
    assembleIndirect(regNumber);
    assembleRegisterDirect(regNumber);

    return 0;
}

