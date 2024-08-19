#define MAX_LINE_LENGTH 81
#define MAX_LABEL_NAME 32
#define CODE_SEGMENT_SIZE 16
#define CODE_EE_SIZE 13
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
extern const char NOT_FOUND[];
extern const int registers[];

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
}External;

External *externs;
extern int externCount;

typedef struct {
    int wordCounter;
} L;
extern L *l;

extern int IC;
extern int DC;

int addData(const char *line, int countLine,const char * newSymbolName);

int addString(const char *line, int countLine, const char *newSymbolName);

int externDefinition(const char *line, int countLine);

int entryDefinition(const char *line, int countLine, int isFirstPass);

int addOperation(const char *line,int numOper, int countLine, const char *newSymbolName);

int updateOparand(const char *line, int countLine, int countAdress);

char *changeNameOfFile(const char *sourceFileName,const char *fileNameEnding);

int operationWithTwoOperand(const char *line,const char* newSymbolName, int numOper, int countLine);

int operationWithOneOperand(const char *line, const char *newSymbolName, int numOper, int countLine);

int operationWithoutOperand(const char *line, const char *newSymbolName, int numOper, int countLine);

/*int checkIfLabbleIsData(const char* label);*/

//check if it a num
int isNumber(const char *str);

//check if it is name restricted
int isNameRestricted(const char *name);

//check if it register
int isRegister( const char *name);

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
char* findExtern(const char *externName);

//is restricted name
int isNameRestricted(const char *token);

//find operation
int findOperation(const char *token);

//binary to decimal
int binaryToDecimal(const char *binary);

//decimal to binary
char* decimalToBinary(int n, int numOfChar);

int isCorrectString(const char *name);

//check if the datas write correct
int correctCommas(char *line);