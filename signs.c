/*
 Maman 14 > signs.c
 Created by Avital on 08/03/2018
 */

#include <ctype.h>
#include <string.h>
#include "dataStructure.h"
#include "signs.h"
#include "dataValidation.h"

#define LONGEST_COMMAND 5
#define LONGEST_INSTRUCTION 8

char command_words[COMMAND_WORDS][LONGEST_COMMAND] = {"MOV", "CMP", "ADD", "SUB", "NOT", "CLR", "LEA", "INC", "DEC", "JMP", "BNE", "RED", "PRN", "JSR", "RTS", "STOP"};
char instruction_words[INSTRUCTION_WORDS][LONGEST_INSTRUCTION] = {".DATA", ".STRING", ".STRUCT", ".ENTRY", ".EXTERN"};
char registers[REGISTERS][3] = {"r0", "r1", "r2", "r3", "r4", "r5", "r6", "r7"};

/**************************************************************************************/
/* The function gets a pointer to string and checks if it is command or instruction word */

void analyze_word(struct word *w)
{
    if (get_command_code(w->string)>-1)
        w->type = COMMAND;
        
    else if (get_instruction_code(w)>-1)
        w->type = INSTRUCTION;
}

/**************************************************************************************/
/* The function gets a pointer to string and checks if it is a command code
 it returns the found command code ID, or -1 if not found */

int get_command_code (char *wordToCheck)
{
    int i;
    char command[LONGEST_COMMAND]={0};
    char word[LONGEST_COMMAND]={0};
    

    memcpy(word,wordToCheck,LONGEST_COMMAND);
           
    for (i=0; i<LONGEST_COMMAND; i++)
           word[i] = toupper(word[i]);
    
    for (i=0; i<COMMAND_WORDS; i++)
    {
        memcpy(command,command_words[i],LONGEST_COMMAND);
        
        if (strcmp(word,command)==0)
            return i;
        
    }
    return -1;
}

/**************************************************************************************/
/* The function gets a pointer to string and checks if it is an instruction code
 it returns the found instruction code ID, or -1 if not found */

int get_instruction_code (word *wordToCheck)
{
    int i;
    char instruction[LONGEST_INSTRUCTION]={0};
    char word[LONGEST_INSTRUCTION]={0};
    
    memcpy(word,wordToCheck->string,LONGEST_INSTRUCTION);
    
    for (i=0; i<LONGEST_INSTRUCTION; i++)
        word[i] = toupper(word[i]);
    
    for (i=0; i<INSTRUCTION_WORDS; i++)
    {
        memcpy(instruction,instruction_words[i],LONGEST_INSTRUCTION);
    
    if (strcmp(word,instruction)==0)
        return i;
    }
    return -1;
}

/**************************************************************************************/
/* The function gets a pointer to word struct and checks which addressing type it is.
 it returns the found addressing type ID, or -1 if not found */

int get_addressing_type(word *word)
{
    if (is_register(word))
        return DIRECT_REGISTER;
    else
        if (is_struct_targeting(word,0))
            return STRUCT_ACCESS;
        else
            if (is_immediate_targeting(word))
                return IMMEDIATE;
            else
                if (is_label(word->string, 0))
                    return DIRECT;
    
    return -1;
}

/**************************************************************************************/
/* The function gets a pointer to string and checks if it is immediate targeting
 it returns 1 - for immediate, 0 - not immediate */

int is_immediate_targeting(word *word)
{
    if (word->string[0]=='#')
    {
        char tmp[MILA_SIZE] = {0};
        memcpy(tmp, &word->string[1], word->length-1);
        
        if (is_number(tmp))
            return 1;
    }
    return 0;
}

/**************************************************************************************/
/* The function gets a pointer to string and checks if it is struct targeting
 it returns 1 - for struct, 0 - not struct */

int is_struct_targeting(word *word, int is_declaration)
{
    char string[word->length];
    char suffix[3]={0};
    char i;
    char j;
    
    if(is_declaration && get_instruction_code(word) == 2)
        return 1;
    
    strcpy(string,word->string);
    
    if(word->length == 1)
        return 0;
    
    for (i=word->length-2, j=0; i<word->length; i++)
        suffix[j++] = string[i];
    
    if (strcmp(suffix, ".1")==0 || strcmp(suffix, ".2")==0)
        return 1;
    
    return 0;
}

/**************************************************************************************/
/* The function gets a pointer to string and checks if it is a valid register name
 it returns 1 - for valid register, 0 - not register */

int is_register(word *word)
{
    int i;
    
    for (i=0; i<REGISTERS; i++)
        if (strcmp(word->string, registers[i])==0)
            return 1;
    
    return 0;
}

