/*
 Maman 14 > second_iteration.c
 Created by Avital on 28/03/2018.
 */

#include <string.h>
#include <stdlib.h>
#include "dataStructure.h"
#include "dataValidation.h"
#include "utils.h"
#include "second_iteration.h"


extern struct assembler_output *codeTable;
extern struct assembler_output *dataTable;
extern struct label *labels;
extern struct label *usedLabels;
extern struct label *entryLabels;
extern struct label *declaredExternalLabels;

void fill_missing_codeTable_details(void);
void fill_missing_usedLabels_type(void);
struct label *search_regular_label(char *labelToSearch);
struct label *search_extern_label(char *labelToSearch);
struct label *search_entry_label(char *labelToSearch);
int has_entry_privilege(char *labelToSearch);

/**************************************************************************************/
/* The function looks for used labels, and fills their relevant ERA status in the codeTable */

void prepare_for_second_iteration(void)
{
    fill_missing_codeTable_details();
    fill_missing_usedLabels_type();
}

/**************************************************************************************/
/* The function looks for used labels, and fills their relevant ERA status in the codeTable */

void fill_missing_codeTable_details(void)
{
    struct assembler_output *ptr_codeTable = codeTable;
    
    while (ptr_codeTable != NULL)
    {
        if (ptr_codeTable->is_using_label)
        {
            struct label *declarationOfLabel = search_regular_label(ptr_codeTable->used_label);
            
            if (declarationOfLabel != NULL)
            {
                int binaryCode [MILA_SIZE-ERA_SIZE] = {0};
                int binary_ERA [BITS_0to5_SIZE]={0};
                int combinedBinary [MILA_SIZE] = {0};
                
                translate_from_base_10(declarationOfLabel->dAddress, BINARY_BASE, binaryCode, 0, 1, MILA_SIZE-ERA_SIZE);
                put_binary_code_in_place(binaryCode, ERA_SIZE, MILA_SIZE-1, combinedBinary);
                
                translate_from_base_10(RELOCATABLE, BINARY_BASE, binary_ERA, 0, 1,BITS_0to5_SIZE);
                put_binary_code_in_place(binary_ERA, 0, ERA_SIZE-1, combinedBinary);
                
                memcpy(ptr_codeTable->binaryCode, combinedBinary, MILA_SIZE*sizeof(int));
            }
            
            /* Its an external label */
            else
            {
                struct label *searchedExternLabel = search_extern_label(ptr_codeTable->used_label);
                
                if(searchedExternLabel != NULL)
                {
                    translate_from_base_10(ptr_codeTable->dAddress, WEIRD_BASE, ptr_codeTable->base32Address, 0, 1, WEIRD_BASE_32_SIZE);
                    translate_from_base_10(EXTERNAL, BINARY_BASE, ptr_codeTable->binaryCode, 0, 0, MILA_SIZE);
                }
                
                else
                {
                    struct label *searchedEntryLabel = search_entry_label(ptr_codeTable->used_label);
                    
                    if (searchedEntryLabel != NULL)
                    {
                        int binaryCode[MILA_SIZE] = {0};
                        int binary_ERA [BITS_0to5_SIZE]={0};
                        int combinedBinary[MILA_SIZE] = {0};
                        
                        translate_from_base_10(declarationOfLabel->dAddress, BINARY_BASE, binaryCode, 0, 1, MILA_SIZE-ERA_SIZE);
                        put_binary_code_in_place(binaryCode, ERA_SIZE, MILA_SIZE-1, combinedBinary);
                        
                        translate_from_base_10(RELOCATABLE, BINARY_BASE, binary_ERA, 0, 1, BITS_0to5_SIZE);
                        put_binary_code_in_place(binary_ERA, 0, ERA_SIZE-1, combinedBinary);
                        
                        memcpy(ptr_codeTable->binaryCode, combinedBinary, MILA_SIZE);
                    }
                    else
                    {
                        char failure[ERROR_DESCRIPTION_LENGTH] = {0};
                        sprintf(failure,"Label \"%s\" was not found in file or as an extern label", ptr_codeTable->used_label);
                        error_handler(failure,ptr_codeTable->origin_lineID);
                    }
                }
            }
        }
        ptr_codeTable = ptr_codeTable->next;
    }
}

/**************************************************************************************/
/* The function looks for used labels, and fills their relevant type (External or Entry) */

void fill_missing_usedLabels_type(void)
{
    struct label *usedLabel = usedLabels;
    struct label *declaredLabel = labels;
    
    while (usedLabel != NULL)
    {
        if(search_extern_label(usedLabel->string) != NULL)
            usedLabel->type = EXT;
        usedLabel = usedLabel->next;
        
    }
    
    while (declaredLabel != NULL)
    {
        if(search_entry_label(declaredLabel->string) != NULL)
            declaredLabel->type = ENT;
        declaredLabel = declaredLabel->next;
    }
}


/**************************************************************************************/
/* The function gets a pointer to string representing a label name to search,
 and looks up for it in the declared labels table.
 It returns: NULL - if the label wasn't found, or *p - pointer to the relevant node in labels */

struct label *search_regular_label(char *labelToSearch)
{
    struct label *p = new_label;
    p = labels;
    
    while (p != NULL)
    {
        if (strcmp(p->string, labelToSearch) == 0)
            return p;
        p = p->next;
    }
    return NULL;
}

/**************************************************************************************/
/* The function gets a pointer to string representing a label name to search,
 and looks up for it in the externalLabels table.
 It returns: NULL - if the label wasn't found, or *p - pointer to the relevant node in externalLabels */

struct label *search_extern_label(char *labelToSearch)
{
    struct label *p = new_label;
    p = declaredExternalLabels;
    
    while (p != NULL)
    {
        if (strcmp(p->string, labelToSearch) == 0)
            return p;
        p = p->next;
    }
    return NULL;
}

/**************************************************************************************/
/* The function gets a pointer to string representing a label name to search,
 and looks up for it in the entryLabels table.
 It returns: NULL - if the label wasn't found, or *p - pointer to the relevant node in entryLabels */

struct label *search_entry_label(char *labelToSearch)
{
    struct label *p = new_label;
    p = entryLabels;
    
    while (p != NULL)
    {
        if (strcmp(p->string, labelToSearch) == 0)
            return p;
        p = p->next;
    }
    return NULL;
}

/**************************************************************************************/
/* The function gets a pointer to string representing a label name to search (assuming it is an extern label),
 and looks up for it in the entryLabels table.
 It returns: 0 - if the label has no entry privilege, 1 - if the label is declared as an entry */

int has_entry_privilege(char *labelToSearch)
{
    struct label *p = new_label;
    p = entryLabels;
    
    while (p != NULL)
    {
        if (strcmp(p->string, labelToSearch) == 0)
            return 1;
        p = p->next;
    }
    return 0;
}

