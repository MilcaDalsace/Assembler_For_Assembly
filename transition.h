//first pass
void firstPass(const char *sourceFile);

//second pass
void secondPass(const char *sourceFile);

//build output files
void buildOutputFiles(const char *sourceFile);

//Check if the name of lable is correct
int labbelNameIsCorrect(char *newSymbolName, int *uncorrect, int *countLine);