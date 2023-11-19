/*
 Maman 14 > signs.h
 Created by Avital on 08/03/2018
 */

#ifndef signs_h
#define signs_h

#include <stdio.h>
#include "dataStructure.h"

#define COMMAND_WORDS 16
#define INSTRUCTION_WORDS 5
#define REGISTERS 8

enum targeting_opedands {IMMEDIATE, DIRECT, STRUCT_ACCESS, DIRECT_REGISTER};
enum coding_type {ABSOLUTE, EXTERNAL, RELOCATABLE};
enum instruction_type {DATA,STRING,STRUCT,ENTRY,EXTERN};
enum command_type {MOV,CMP,ADD,SUB,NOT,CLR,LEA,INC,DEC,JMP,BNE,RED,PRN,JSR,RTS,STOP};

static char weird_alphabet[WEIRD_BASE] = {'!', '@', '#', '$', '%', '^', '&', '*', '<', '>', 'a', 'b', 'c', 'd', 'e', 'f', 'g', 'h', 'i', 'j', 'k', 'l', 'm', 'n', 'o', 'p', 'q', 'r', 's', 't', 'u', 'v'};

typedef struct line line;
typedef struct word word;
typedef struct assembler_output assembler_output;
typedef struct label label;


/* The function gets a pointer to string and checks if it is a command code
 it returns the found command code ID, or -1 if not found */
int get_command_code (char *wordToCheck);

/* The function gets a pointer to string and checks if it is an instruction code
 it returns the found instruction code ID, or -1 if not found */
int get_instruction_code (word *word);

/* The function gets a pointer to string and checks if it is struct targeting
 it returns 1 - for struct, 0 - not struct */
int is_struct_targeting(word *word, int is_declaration);

/* The function gets a pointer to string and checks if it is immediate targeting
 it returns 1 - for immediate, 0 - not immediate */
int is_immediate_targeting(word *word);

/* The function gets a pointer to string and checks if it is a valid register name
 it returns 1 - for valid register, 0 - not register */
int is_register(word *word);

/* The function gets a pointer to word struct and checks which addressing type it is.
 it returns the found addressing type ID, or -1 if not found */
int get_addressing_type(word *word);

/* The function gets a pointer to string and checks if it is command or instruction word */
void analyze_word(struct word *w);

#endif /* signs_h */

