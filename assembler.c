/*
 Maman 14 > assembler.c
 Created by Avital on 10/03/2018.
 */

#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <stdlib.h>
#include "dataStructure.h"
#include "utils.h"
#include "dataValidation.h"
#include "first_Iteration.h"
#include "second_iteration.h"
#include "assembler.h"

void free_labels(label *head);
void free_assembler_output(assembler_output *head);
void free_memory(void);
void create_files(char *fileName);
void create_obj_file(char *fileName);
void create_ent_file(char *fileName);
void create_ext_file(char *fileName);

extern struct assembler_output *codeTable;
extern struct assembler_output *dataTable;
extern struct label *labels;
extern struct label *usedLabels;
extern struct label *entryLabels;
extern struct label *declaredExternalLabels;

/* assembler main function (called from main.c)/*/
void assembler(char *fileName)
{
    FILE *file;
    char fileToRead[WORD_SIZE];
    
    /* get file name with filetype */
    strcpy(fileToRead, fileName);
    strcat(fileToRead, ".as");
    
    printf("Opening file: %s \n",fileToRead);
    errno = 0;
    
    if ((file = fopen(fileToRead, "r"))) /* if opening success */
    {
        printf("Translating file %s\n",fileToRead);
        prepare_for_first_iteration(file);
        printf("Finished first iteration\n");
        prepare_for_second_iteration();
        create_files(fileName);
        free_memory();
        fclose(file);
        printf("Closing file: %s \n",fileToRead);
        
    } else {
        printf("Error code %d: Cannot open file: %s - %s \n", errno, fileToRead, strerror(errno));
    }
}

/**************************************************************************************/

/* The function gets a filename.
 In case the .as file had no errors, this function creates relevant output files (.obj, .ent, .ext).
 Otherwise, no output files will be created for the origin .as file. */

void create_files(char *fileName)
{
    
    if (has_errors())
        printf("\n\nERROR: The file contains errors. Fix the failures and try again. \n");
    else
    {
        create_obj_file(fileName);
        create_ent_file(fileName);
        create_ext_file(fileName);
    }
    
}

/**************************************************************************************/

/* The function gets a filename (.as file) and creates a relevant .obj file */

void create_obj_file(char *fileName)
{
    FILE *file;
    char objFile[WORD_SIZE];
    strcpy(objFile, fileName);
    strcat(objFile, ".ob");
    file = fopen(objFile, "w+");
    errno = 0;
    
    if (file == NULL)
        printf("Error code %d: Cannot open file: %s - %s \n", errno, objFile, strerror(errno));
    else
    {
        struct assembler_output *pCode = codeTable;
        struct assembler_output *pData = dataTable;
        struct weird_output *output = (struct weird_output *)malloc(sizeof(struct weird_output));
        int * binaryIn32 = (int *)malloc(10*WEIRD_BASE_32_SIZE*sizeof(int));
        int * binaryInDecimals = (int *)malloc(sizeof(int));
        
        /* Print to file the data table*/
        while (pCode != NULL)
        {
            memset(binaryIn32,0,10*MILA_SIZE*sizeof(int));
            memset(binaryInDecimals,0,sizeof(int));
            
            translate_to_base_10(pCode->binaryCode, BINARY_BASE, binaryInDecimals, MILA_SIZE);
            translate_from_base_10(*binaryInDecimals,WEIRD_BASE, binaryIn32, 0, 1, WEIRD_BASE_32_SIZE);
            
            output = translate_to_weird_language(pCode->base32Address, binaryIn32);
            fprintf(file, "%.2s  %.2s\n", output->base32Address, output->base32Code);
            pCode = pCode->next;
        }

        while (pData != NULL)
        {
            memset(binaryIn32,0,10*MILA_SIZE*sizeof(int));
            memset(binaryInDecimals,0,sizeof(int));
            
            translate_to_base_10(pData->binaryCode, BINARY_BASE, binaryInDecimals, MILA_SIZE);
            translate_from_base_10(*binaryInDecimals,WEIRD_BASE, binaryIn32, 0, 1, WEIRD_BASE_32_SIZE);
            
            output = translate_to_weird_language(pData->base32Address, binaryIn32);
            fprintf(file, "%.2s\t%.2s\n", output->base32Address, output->base32Code);
            pData = pData->next;
        }
        
        fclose(file);
    }
        
}

/**************************************************************************************/

/* The function gets a filename (.as file) and creates a relevant .ent file */

void create_ent_file(char *fileName)
{
    FILE *file;
    char newFile[WORD_SIZE];
    label *pointer = labels;
    
    if (pointer != NULL)
    {
        strcpy(newFile, fileName);
        strcat(newFile, ".ent");
        file = fopen(newFile, "w+");
        errno = 0;
    
        if (file == NULL)
            printf("Error code %d: Cannot open file: %s - %s \n", errno, newFile, strerror(errno));
        else
        {

            while (pointer != NULL)
            {
                if (pointer->type == ENT)
                {
                    struct weird_output *output = (struct weird_output *)malloc(sizeof(struct weird_output));
                    int * binaryIn32 = (int *)malloc(WEIRD_BASE_32_SIZE*sizeof(int));
                    
                    memset(binaryIn32,0,WEIRD_BASE_32_SIZE*sizeof(int));
                    output = translate_to_weird_language(pointer->base32Address, binaryIn32);
                    fprintf(file, "%-30s  %.2s\n",  pointer->string, output->base32Address);
                }
                pointer = pointer->next;
            }
            
            fclose(file);
        }
    }
}

/**************************************************************************************/

/* The function gets a filename (.as file) and creates a relevant .ext file */

void create_ext_file(char *fileName)
{
    FILE *file;
    char newFile[WORD_SIZE];
    label *pointer = usedLabels;
    
    if (pointer != NULL)
    {
        strcpy(newFile, fileName);
        strcat(newFile, ".ext");
        file = fopen(newFile, "w+");
        errno = 0;
        
        if (file == NULL)
            printf("Error code %d: Cannot open file: %s - %s \n", errno, newFile, strerror(errno));
        else
        {
            
            while (pointer != NULL)
            {
                if (pointer->type == EXT)
                {
                    struct weird_output *output = (struct weird_output *)malloc(sizeof(struct weird_output));
                    int * binaryIn32 = (int *)malloc(WEIRD_BASE_32_SIZE*sizeof(int));
                    
                    memset(binaryIn32,0,WEIRD_BASE_32_SIZE*sizeof(int));
                    output = translate_to_weird_language(pointer->base32Address, binaryIn32);
                    fprintf(file, "%-30s  %.2s\n",  pointer->string, output->base32Address);
                }
                pointer = pointer->next;
            }
            
            fclose(file);
        }
    }
}

/**************************************************************************************/

/* The function frees all the tables created in the memory during the run of the porogram */

void free_memory()
{
    free_assembler_output(codeTable);
    free_assembler_output(dataTable);
    
    free_labels(labels);
    free_labels(usedLabels);
    free_labels(entryLabels);
    free_labels(declaredExternalLabels);
}

/**************************************************************************************/

/* The function gets a pointer to head of assembler_output struct, and frees its memory */

void free_assembler_output(assembler_output *head)
{
    assembler_output *p;
    
    while (head != NULL)
    {
        p = head;
        head= head->next;
        free (p);
    }
}

/**************************************************************************************/

/* The function gets a pointer to head of label struct, and frees its memory */

void free_labels(label *head)
{
    label *p;
    
    while (head != NULL)
    {
        p = head;
        head= head->next;
        free (p);
    }
}
