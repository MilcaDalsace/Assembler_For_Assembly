/*בדיקה האם מספר הקבצים שהגיעו תקין*/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "preAssembler.h"

int numFileIsCoorect(int argc, char *argv[])
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
    }
    else
        return 1;
}

int main(int argc, char *argv[])
{
    if (numFileIsCoorect(argc, argv))
    {
        int i;
        for (i = 1; i < argc; i++)
        {
            processFile(argv[i]);
        }
    }
    int i;
    for (i = 0; i < macroCount; i++)
    {
        free(macros[i].name);
        free(macros[i].content);
    }
    free(macros);
    return 0;
}