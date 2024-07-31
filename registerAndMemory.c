#include <stdio.h>
#include <stdint.h>

#define MEMORY_SIZE 4096
#define REGISTER_SIZE 15

typedef int16_t Word;  // כל תא בזיכרון ובאוגר הוא בגודל 15 סיביות

typedef struct {
    Word registers[8];   // 8 אוגרים כלליים
    Word memory[MEMORY_SIZE];  // זיכרון בגודל 4096 תאים
    Word PSW;  // אוגר מצב מעבד (Program Status Word)
} CPU;

// אתחול ה-CPU
void initCPU(CPU *cpu) {
    for (int i = 0; i < 8; i++) {
        cpu->registers[i] = 0;
    }
    for (int i = 0; i < MEMORY_SIZE; i++) {
        cpu->memory[i] = 0;
    }
    cpu->PSW = 0;
}

// הדפסת מצב הרגיסטרים
void printRegisters(CPU *cpu) {
    for (int i = 0; i < 8; i++) {
        printf("r%d: %d\n", i, cpu->registers[i]);
    }
    printf("PSW: %d\n", cpu->PSW);
}

// דוגמה לפונקציה שמבצעת פעולת חיבור על שני רגיסטרים ומעדכנת את ה-PSW
void add(CPU *cpu, int reg1, int reg2, int dest) {
    cpu->registers[dest] = cpu->registers[reg1] + cpu->registers[reg2];
    // עדכון דגלים ב-PSW לפי תוצאה (כאן לדוגמה מתעדכנים רק דגל ה-carry ודגל ה-zero)
    if (cpu->registers[dest] == 0) {
        cpu->PSW |= 0x01;  // דגל zero
    } else {
        cpu->PSW &= ~0x01;  // איפוס דגל zero
    }
    if (cpu->registers[dest] & 0x8000) {
        cpu->PSW |= 0x02;  // דגל carry
    } else {
        cpu->PSW &= ~0x02;  // איפוס דגל carry
    }
}

int main() {
    CPU cpu;
    initCPU(&cpu);

    // דוגמה לשימוש
    cpu.registers[0] = 5;
    cpu.registers[1] = 10;
    add(&cpu, 0, 1, 2);

    printRegisters(&cpu);

    return 0;
}