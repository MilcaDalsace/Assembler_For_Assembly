#define MAX_LINE_LENGTH 81
#define MAX_LABEL_NAME 32
#define CODE_SEGMENT_SIZE 15
#define CODE_SIZE 4
#define ARE_SIZE 3

// Constant strings for addressing methods
extern const char IMMEDIATE_ADDRESS[];
extern const char DIRECT_ADDRESS[];
extern const char INDIRECT_HOARD_ADDRESS[];
extern const char DIRECT_HOARD_ADDRESS[];
extern const char ABSOLUTE[];
extern const char RELOCATABLE[];
extern const char EXTERNAL[];
extern const char ZERO[];

typedef struct {
    char symbol[MAX_LINE_LENGTH];
    char code[CODE_SEGMENT_SIZE];
    char externName[MAX_LABEL_NAME];
    int address;
    int isData;
    int isEntry;
    int isExtern;
}Symbol;

extern Symbol *symbols;
extern int symbolCount;

typedef struct {
    char *name;
    int count;
}SymbolTable;

extern SymbolTable *symbolTable;
extern int labelCount;

typedef struct {
    char *name;
}Extern;
Extern *externs;

extern int externCount;

typedef struct {
    int wordCounter;
} L;
extern L *l;

extern int IC;
extern int DC;

//check if it a num
int isNumber(const char *str);

//check if it is name restricted
int isNameRestricted(const char *name);

//check if it register
int isRegister(const char *name);

//find method addressing
char* findsMethodOfAddressing(const char *operand);

//miun operation
char* miunOperation(int num, const char *operand1, const char *operand2);

//miun operand
char* miunOperand(const char *operand, int firstOperand);

//iun two registers
char* miunTwoRegister(const char *operand1, const char *operand2);

//add symbol
void addSymbol(const char *symbolName, const char *externName, const char *code, const int isData, const int isEntry, const int isExtern);

//add labbel
void addLabbel(const char *name, const int count);

//add extern
void addExtern(const char *name);

//add L
void addL(int counteLine, int wordCounter);

//check if it a labbel definition
char* labelDefinition(const char *token);

//find labbel
int findLabbel(const char *token);

//find extern
Extern* findExtern(const char *token);

//is restricted name
int isNameRestricted(const char *token);

//find operation
int findOperation(const char *token);

//binary char to int
int binaryCharToInt(char c);

//binary to decimal
int binaryToDecimal(const char *binary);

//decimal to binary
char* decimalToBinary(int n, int numOfChar);

//deecimmal to octal
void decimalToOctal(int decimal, char *octal);

//binary to octal
void binaryToOctal(const char *binary, char *octal);

//check if the datas write correct
int correctCommas(char *line);
