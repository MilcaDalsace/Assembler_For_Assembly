#define MAX_LINE_LENGTH 81
#define MAX_MACRO_NAME 77

typedef struct
{
    char *name;
    char *content;
} Macro;

extern Macro *macros;
extern int macroCount;

/*is name restricted האם השם שמור?*/
/*int isNameRestricted(const char *name);*/

/*יצירת מקרו חדש והוספתו לרשימת המקרוים*/
void addMacro(const char *name, const char *content) ;

/*מחזירה את קוד המקרו לפי השם של*/
const char* findMacro(const char *name);

/*בדיקה האם הקובץ נפתח בצורה נכונה*/
int checkingWhetherTheFileIsCorrect(FILE * checkedFile);

/*מחיקת קובץ ויציאה*/
void removeFileAndExit(const char *outputFilename);

/*פריסת הקוד של הפרה אסמבלר ויוצרת את הקובץ החדש */
void processFile(const char *inputFilename);
