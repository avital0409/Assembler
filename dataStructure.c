/*
 Maman 14 > dataStructure.c
 Created by Avital on 08/03/2018
 */

#include "dataStructure.h"
#include <string.h>
#include <stdlib.h>
#include <ctype.h>


int add_NodeTo_labels(struct label *newLabel);

struct assembler_output *codeTable;
struct assembler_output *dataTable;
struct label *labels;
struct label *usedLabels;
struct label *entryLabels;
struct label *declaredExternalLabels;


/* The function gets a pointer to a line struct, and a pointer to the whole line as a string.
 It fills the structs' words' data (string, length, hasLabel) and wordsCount for the line*/

void line_to_words(struct line *line_content, char *line)
{
    char *word;
    int i;
    
    remove_excess_spaces(line);
           
    word = strtok(line, ", \t\n");
    for (i = 0; word != NULL; i++)
    {
        line_content->word[i]->length = (int)strlen(word);

        if (i == 0 && word[strlen(word)-1] == ':')
        {
            memcpy(line_content->word[i]->string,word,strlen(word)-1);
            line_content->word[i]->string[strlen(word)-1]='\0';
            line_content->hasLabel = 1;
            line_content->word[i]->length = (int)strlen(word)-1;
        }
        else
            strcpy(line_content->word[i]->string,word);
        
        word = strtok(NULL, " ,\t\n");
    }
    
    line_content->wordsCount = i;
}

/**************************************************************************************/

/* The function gets a pointer to a label (as a word struct), and its decimal address.
 It converts the word to a label struct and adds it to the declared labels list */

int keepLabel(struct word *labelWord,int decimalAddress)
{
    if((get_command_code(labelWord->string)!=-1) || get_instruction_code(labelWord)!=-1 || is_register(labelWord))
        /* if it is a reserved word */
        return -2;
    
    if (labelWord->length > LABEL_SIZE)
        return -3;


    struct label *newLabel = new_label;
    
    * newLabel = (struct label) {
        .string = {0} ,
        .dAddress = decimalAddress ,
        .base32Address = new_base32Address ,
        .using_line = 0 ,
        .type = REGULAR ,
        .next = NULL};
    
    strncpy(newLabel->string, labelWord->string,LABEL_SIZE);
    translate_from_base_10(decimalAddress, WEIRD_BASE, newLabel->base32Address, 0, 1, WEIRD_BASE_32_SIZE);
    
    if (!add_NodeTo_labels(newLabel))
        return -1;
    return 1;
}

/**************************************************************************************/

/* The function gets a pointer to a label struct.
 It adds it to the declared labels list */

int add_NodeTo_labels(struct label *newLabel)
{
    if (labels == NULL)
    {
        labels = new_label;
        labels =  newLabel;
        labels->next = NULL;

    }
    else
    {
        label *p  = new_label;
        p = labels;
        
        while (p->next != NULL)
        {
            if (strcmp(newLabel->string, p->string) == 0)
                return 0;
            p = p->next;
        }

        p->next = newLabel;
        p->next->next = NULL;
    }
    
    return 1;
}

/**************************************************************************************/

/* The function gets a pointer to a label struct.
 It adds it to the declared label entries list */

void add_NodeTo_entryLabels(struct label *newEntry)
{
    if (entryLabels == NULL)
    {
        entryLabels = new_label;
        entryLabels = newEntry;
        entryLabels->next = NULL;
    }
    else
    {
        struct label *p = new_label;
        p = entryLabels;
        
        while (p->next != NULL)
            p = p->next;
        
        p->next = newEntry;
        p->next->next = NULL;
    }
}

/**************************************************************************************/

/* The function gets a pointer to a label struct.
 It adds it to the declared external labels list */

void add_NodeTo_externalLabels(struct label *newExternal)
{
    if (declaredExternalLabels == NULL)
    {
        declaredExternalLabels = new_label;
        declaredExternalLabels = newExternal;
        declaredExternalLabels->next = NULL;
    }
    else
    {
        struct label *p = new_label;
        p = declaredExternalLabels;
        
        while (p->next != NULL)
            p = p->next;
        
        p->next = newExternal;
        p->next->next = NULL;
    }
}

/**************************************************************************************/

/* The function gets a pointer to a assembler output struct.
 It adds it to the code table */

void add_NodeTo_codeTable(struct assembler_output *newOutputLine)
{
    
    if (codeTable == NULL)
    {
        codeTable = new_ao_line;
        codeTable = newOutputLine;
        codeTable->next = NULL;
    }
    else
    {
        struct assembler_output *p = new_ao_line;
        p = codeTable;
        
        while (p->next != NULL)
            p = p->next;
    
        p->next = newOutputLine;
        p->next->next = NULL;
    }
}

/**************************************************************************************/

/* The function gets a pointer to a assembler output struct.
 It adds it to the data table */

void add_NodeTo_dataTable(struct assembler_output *newOutputLine)
{
    if (dataTable == NULL)
    {
        dataTable = new_ao_line;
        dataTable = newOutputLine;
        dataTable->next = NULL;
    }
    else
    {
        struct assembler_output *p = new_ao_line;
        p = dataTable;
        
        while (p->next != NULL)
            p = p->next;
        
        p->next = newOutputLine;
        p->next->next = NULL;
    }
}

/**************************************************************************************/

/* The function gets a pointer to a label struct.
 It adds it to the used (= called) labels list */

void add_NodeTo_usedLabels(struct label *label)
{
    if (usedLabels == NULL)
    {
        usedLabels = new_label;
        usedLabels = label;
        usedLabels->next = NULL;
    }
    else
    {
        struct label *p = usedLabels;

        while (p->next != NULL)
            p = p->next;
        
        p->next = label;
        p->next->next = NULL;
    }
}

/**************************************************************************************/

/* The function gets a pointer to a line as a string.
 It removes all the excess spaces from the line */

void remove_excess_spaces(char *line)
{
    int i;
    int x;
    
    for(i = x = 0; line[i]; ++i)
        if(!isspace(line[i]) || (i>0 && !isspace(line[i-1])))
            line[x++] = line[i];
    line[x] = '\0';
}

