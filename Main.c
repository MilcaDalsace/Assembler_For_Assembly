#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "transition.h"
#include "function.h"

int numFileIsCorect(int argc, char *argv[])
{
    int count = 1;
    while (argv[count] != NULL)
    {
        count++;
    }
    if (argc != (count))
    {
        fprintf(stderr, "Usage: %s <source-file>\n", argv[0]);
        return 0;
    }else return 1;
}

int main(int argc, char *argv[])
{
    if (!numFileIsCorect(argc, argv))
    {
	return 0;
    }
	
        int i;
        for (i = 1; i < argc; i++)
        {
	   // FILE *sourceFile = fopen(argv[i], "r");
	    // First pass
	    firstPass(argv[i]);

	    // Second pass
	    secondPass(argv[i]);

	    // Build output files
	    buildOutputFiles(argv[i]);

	}

    // Free allocated memory
    for (i = 0; i < symbolCount; i++) {
        free(symbolTable[i].name);
    }
    free(symbolTable);
    for (i = 0; i < externCount; i++) {
        free(externs[i].name);
    }
    free(externs);
    free(symbols);

    return 0;
}
// End of main


