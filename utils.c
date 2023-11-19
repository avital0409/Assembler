/*
 Maman 14 > utils.c
 Created by Avital on 08/03/2018
 */

#include <stdlib.h>
#include <stdio.h>
#include <ctype.h>
#include <string.h>
#include "dataStructure.h"
#include "utils.h"
#include "signs.h"
#include "dataValidation.h"


void handle_fieldID(char *fieldID);
void handle_negative_number(int *number, int num_of_representation_digits, int is_flipped);
int is_blank_line(char *line);
int is_comment_line (char *line);
int my_strchr(char cs[], int c);
int word_has_comma(char *string);
void flip_binary(int *binaryCode, int num_of_representation_digits);

/**************************************************************************************/
/* The function gets a pointer to string representing a whole line and checks if the line should be skipped.
 It returns 1 - for skip, 0 - not skip */

int should_skip_line(char *line) {
    return (!((is_blank_line(line)) || (is_comment_line(line))));
}

/**************************************************************************************/
/* The function gets a pointer to string representing a whole line and checks if it is a blank line.
 It returns 1 - for blank, 0 - not blank */

int is_blank_line(char *line)
{
    char *p;
    
    p = line;
    if (*p == '\n')
        return 1;
    else{
        for (; *p ==' '; p++){}
        
        if (*p =='\n')
            return 1;
    }
    return 0;
}

/**************************************************************************************/
/* The function gets a pointer to string representing a whole line and checks if it is a comment line.
 It returns 1 - for comment, 0 - not comment */

int is_comment_line (char *line)
{
    return (*line == ';');
}

/**************************************************************************************/
/* The function gets a string and a char, and searches for the char whithin the string.
 It returns the index of the found c or -1 if not found */

int my_strchr(char cs[], int c)
{
        int i;
        for (i=0; (i < strlen(cs)); i++)
            if (*&cs[i]==c)
                return i;
        return -1;
}

/**************************************************************************************/
/* The function gets a pointer to a string and determines of the word has comma.
 It returns 1 - for comma, 0 - no commas */

int word_has_comma(char *string)
{
    int i;
    
    for (i=0; string[i]!='\0'; i++)
        if (string[i]==',')
            return 1;
    return 0;
}

/**************************************************************************************/
/* The function gets a char and tells if it is a white space (space or tab)
 It returns 1 - for white space, 0 - not white space */

int is_white_space(char character)
{
    if (character == ' ' || character == '\t')
        return 1;
    else
        return 0;
}

/**************************************************************************************/
/* The function gets a pointer to a string representing label name and an int representing if the questioned label
 is asked as a declared label or not. It returns 1 - for label, 0 - not label */

int is_label(char *word, int is_declaration)
{
    int wordLength;
    wordLength = (int)strlen(word);
    
    return ((wordLength <= (LABEL_SIZE)) && (is_declaration == (word[wordLength - 1] == ':')) && (isalpha(word[0])));
    /* valid label length & if declared, there should be ':' & first letter must be an alpha letter */
}

/**************************************************************************************/
/* The function gets a pointer to a binary number, the wanted represented digits and flipping status.
 it turns the number to negative, using 2's completement method */

void handle_negative_number(int *number, int num_of_representation_digits, int is_flipped)
{
    int i;
    int tmp[num_of_representation_digits];
    int fix_index;
    
    memset(tmp,0,num_of_representation_digits*sizeof(int));
    
    for (i = 0; i < num_of_representation_digits; i++)
        number[i] = (number[i] == 0) ? 1 : 0;
    
    fix_index = (is_flipped == 1) ? (i-1) : 0;
    
    number[fix_index] = 1;
}

/**************************************************************************************/
/* The function replaces source binary code in binary output, the range of the bits to put the code in (from - to) */

void put_binary_code_in_place(int * source, int from_index, int to_index, int * binary_output)
{
    int i;
    int j = to_index-from_index;
    int t = 0;
    int * tmp = (int *)malloc(MILA_SIZE*sizeof(int)*10);
    
    memset(tmp, 0,MILA_SIZE*sizeof(int)*10);
    
    for (i = 0; i < MILA_SIZE; i++)
    {
        if (i<from_index || i > to_index)
            /* before and after the range to place the binary, leave the original binary code as it is */
            t = binary_output[i];

        else
            if (i >= from_index && i <= to_index)
                /* in the range, put the new value */
                t = source[j--];
        memcpy(&tmp[i], &t,sizeof(int));
    }
    
    memcpy(binary_output,tmp,MILA_SIZE*sizeof(int));

}

/**************************************************************************************/
/* The function converts to "convert_to_base" from decimal base. */

void translate_from_base_10 (int value_to_convert, int convert_to_base, int * converted_value, int is_negative, int needs_flipping, int num_of_representation_digits)
{
    int i = 0;
    int * tmp = (int *)malloc(num_of_representation_digits*sizeof(int));
    int converted_number[num_of_representation_digits];
    
    memset(tmp, 0,num_of_representation_digits*sizeof(int));
    memset(converted_number,0,num_of_representation_digits*sizeof(int));
    
    while (value_to_convert > 0)
    {
        converted_number[i++] = value_to_convert % convert_to_base;
        value_to_convert /= convert_to_base;
    }
    
    if (needs_flipping)
        flip_binary(converted_number, num_of_representation_digits);
    
    memcpy(converted_value,converted_number,num_of_representation_digits*sizeof(int));

    if (convert_to_base == BINARY_BASE && is_negative)
        handle_negative_number(converted_value ,num_of_representation_digits, needs_flipping);
}

/**************************************************************************************/
/* The function flips binary code */

void flip_binary(int *binaryCode, int num_of_representation_digits)
{
    int i;
    for (i = 0; i<(num_of_representation_digits/2); i++)
    {
        int * t = {0};
        memcpy(&t,&binaryCode[num_of_representation_digits-i-1],sizeof(int));
        memcpy(&binaryCode[num_of_representation_digits-i-1],&binaryCode[i],sizeof(int));
        memcpy(&binaryCode[i], &t,sizeof(int));
    }
}
/**************************************************************************************/
/* The function converts from "convert_to_base" to decimal base. */

void translate_to_base_10(int *value_to_convert, int convert_from_base, int *converted_value, int length)
{
    int power = 1;
    int num = 0;
    int i;
    
    for (i =  0; i < length; i++)
    {
        num += (value_to_convert[i] * power);
        power *= convert_from_base;
    }
    
    memcpy(converted_value,&num,sizeof(int));
}

/**************************************************************************************/
/* The function converts base 32 to weird base 32 */

void get_weird_characters(int *base32, int length, char *converted_to_weird)
{
    int i;
    
    for (i = 0; i < length; i++)
    {
        int digit = base32[i];
        memcpy(&converted_to_weird[i],&weird_alphabet[digit],sizeof(char));
    }
    
    converted_to_weird[i] = '\0';
}

/**************************************************************************************/
/* The function converts base 32 address and machine code in 32 base to weird adderss, and returns weird output that has them*/

struct weird_output *translate_to_weird_language(int *base32Address , int *binary32Code)
{
    struct weird_output *output = (struct weird_output *)malloc(sizeof(struct weird_output));
    
    memset(output, 0, sizeof(struct weird_output));
    
    get_weird_characters(base32Address, WEIRD_BASE_32_SIZE, output->base32Address);
    get_weird_characters(binary32Code, WEIRD_BASE_32_SIZE, output->base32Code);
    
    return output;
}

