/*
  Maman 14 > main.c
  Created by Avital on 10/03/2018.
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "utils.h"
#include "assembler.h"

int readInput(void);

int main(int argc, char *argv[])
{
    int fileNo;
    
    if (argc>0)
    {
        for (fileNo = 1; fileNo<argc; fileNo++)
        {
            printf("fileName: %s\n", argv[fileNo]);
            assembler(argv[fileNo]);
        }
    }
    
    return 0;

}

/**************************************************************************************/

int readInput()
{
    char buffer[FILE_SIZE_BUFFER];
    int files = 0;
    
    if(fgets(buffer, sizeof(buffer), stdin) != 0)
    {
        char *str = buffer;
        char fileName[WORD_SIZE]; /* 1 char is reserved for \0 & additional 4 chars are reserved for file extension that will be attached later */
        int  nextWordP;
        
        while (sscanf(str, "%250s%n", fileName, &nextWordP) == 1)
        {
            str += nextWordP;/*move to the end of the received string*/
            files++;
            
            if (files == 1)
            {
                if(strcmp(fileName, "assemble") != 0)
                    return -1;
                continue;
            }
            
            printf("fileName: %s\n", fileName);
            assembler(fileName);
        }
        return 0;
    }
    else
        return -1;
}



