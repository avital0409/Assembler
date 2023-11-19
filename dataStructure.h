/*
 Maman 14 > dataStructure.h
 Created by Avital on 08/03/2018
 */
#pragma once

#include <stdio.h>
#include "utils.h"
#include "signs.h"

#define new_ao_line (struct assembler_output *) malloc(10*sizeof(struct assembler_output))
#define new_label (struct label *) malloc(10*sizeof(struct label))
#define new_line (struct line *) malloc(10*sizeof(struct line))
#define new_word (struct word *) malloc(10*sizeof(struct word))
#define new_base32Address (int *)malloc(WEIRD_BASE_32_SIZE*sizeof(int))

typedef enum wordType{
    COMMAND, INSTRUCTION, UNKNOWN
}wordType;

typedef enum labelType{
    REGULAR, ENT, EXT
}labelType;

struct line {
    struct word *word[LINE_SIZE];
    int lineID;
    int wordsCount;
    int hasLabel;
};

struct word {
    char string[LINE_SIZE];
    int position;
    wordType type;
    int length;
};

struct assembler_output {
    char label[LABEL_SIZE];
    int dAddress;
    int *base32Address;
    int binaryCode[MILA_SIZE];
    int base32Code[WEIRD_BASE_32_SIZE];
    int is_using_label;
    char used_label[LABEL_SIZE];
    int origin_lineID;
    struct assembler_output *next;
};

struct label {
    char string[LABEL_SIZE];
    int dAddress;
    int *base32Address;
    int using_line;
    int type;
    struct label *next;
};

extern struct weird_output
{
    char base32Address[WEIRD_BASE_32_SIZE];
    char base32Code[WEIRD_BASE_32_SIZE];
    
    struct obj_output *next;
}weird_output;


/* The function gets a pointer to a line struct, and a pointer to the whole line as a string.
 It fills the structs' words' data (string, length, hasLabel) and wordsCount for the line*/
void line_to_words(struct line *line_content, char *line);

/* The function gets a pointer to a label (as a word struct), and its decimal address.
 It converts the word to a label struct and adds it to the declared labels list */
int keepLabel(struct word *label,int decimalAddress);

/* The function gets a pointer to a label struct.
 It adds it to the declared label entries list */
void add_NodeTo_entryLabels(struct label *newEntry);

/* The function gets a pointer to a label struct.
 It adds it to the declared external labels list */
void add_NodeTo_externalLabels(struct label *newExternal);

/* The function gets a pointer to a assembler output struct.
 It adds it to the code table */
void add_NodeTo_codeTable(struct assembler_output *newOutputLine);

/* The function gets a pointer to a assembler output struct.
 It adds it to the data table */
void add_NodeTo_dataTable(struct assembler_output *newOutputLine);

/* The function gets a pointer to a label struct.
 It adds it to the used (= called) labels list */
void add_NodeTo_usedLabels(struct label *label);

/* The function gets a pointer to a line as a string.
 It removes all the excess spaces from the line */
void remove_excess_spaces(char *line);

