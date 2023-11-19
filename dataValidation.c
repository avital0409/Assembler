/*
 Maman 14 > dataValidation.c
 Created by Avital on 17/03/2018
 */

#include <string.h>
#include <ctype.h>
#include "dataStructure.h"
#include "dataValidation.h"


static int errors_flag = 0;

/* The function gets a pointer to a error description's string and line ID of the error.
 It turns the error flag on and print the error in a specified format */

void error_handler (char * error_description, int failedLine)
{
    errors_flag = 1;
    printf("Error (line %d): %s\n",failedLine, error_description);
}

/**************************************************************************************/

/* The function returns 0 if there were no errors found, and 1 if there are errors for the certain file */

int has_errors()
{
    return errors_flag;
}


/**************************************************************************************/

/* The function gets a pointer to a line struct and a string's poisition in the line.
 It validates if the string has apostrophes 
 and returns 0 - for an invalid string (no apostrophes), 1 - for a valid string */

int validate_string (line *line, int wordPosition)
{
    int string_length = line->word[wordPosition]->length;
    if ((line->word[wordPosition]->string[0] != '\"') || (string_length-1 != '\"'))
    {
        char failure[ERROR_DESCRIPTION_LENGTH];
        sprintf(failure,"String \"%s\" has missing apostrophes", line->word[wordPosition]->string);
        error_handler(failure,line->lineID);
        return 0;
    }
    return 1;
}

/**************************************************************************************/

/* The function gets a pointer to a line struct the line's ID.
 It validates if there are no commas before or after a number, or double commas between 2 numbers
 and returns 0 - for an invalid line, 1 - for a valid line */

int validate_line(char *line, int lineID)
{
    int lineLength = (int)strlen(line);
    
    if (has_comma_before_first_number(line))
    {
        char failure[ERROR_DESCRIPTION_LENGTH];
        sprintf(failure,"There are no numbers before first comma");
        error_handler(failure,lineID);
        return 0;
    }
    if (has_comma_after_last_number(line))
    {
        char failure[ERROR_DESCRIPTION_LENGTH];
        sprintf(failure,"There are no numbers after last comma");
        error_handler(failure,lineID);
        return 0;
    }
    if (has_commas_in_a_row(line,lineLength) == 1)
    {
        char failure[ERROR_DESCRIPTION_LENGTH];
        sprintf(failure,"There are 2 commas in a row");
        error_handler(failure,lineID);
        return 0;
    }
    
    return 1;
}

/**************************************************************************************/

/* The function gets a pointer to a line struct.
 It validates if there are no commas before the first number,
 and returns 0 - for an invalid line, 1 - for a valid line*/

int has_comma_before_first_number(char *line)
{
    char *beforeComma;
    char *word;
    int foundNumber = 0;
    
    beforeComma = strtok(line, ",");
    word = strtok(beforeComma," \n");
    
    while(word != NULL)
    {
        if (is_number(word))
            foundNumber=1;
        word = strtok(NULL, " \n");
    }
    if (foundNumber)
        return 0;
    else
        return 1;
}

/**************************************************************************************/

/* The function gets a pointer to a line struct.
 It validates if there are no commas after the last number,
 and returns 0 - for an invalid line, 1 - for a valid line*/

int has_comma_after_last_number(char *line)
{
    char *pLastComma;
    char *word;
    int foundNumber = 0;
    
    pLastComma = strrchr(line,',');
    
    word = strtok(pLastComma, " \n");
    
    while(word != NULL) {
        if (is_number(word))
            foundNumber=1;
        word = strtok(NULL, " \n");
    }
    return (!foundNumber);
    
}

/**************************************************************************************/

/* The function gets a pointer to a line struct.
 It validates if there are double commas between two numbers
 and returns 0 - for an invalid line, 1 - for a valid line*/

int has_commas_in_a_row(char * line, int line_length)
{
    int i;
    int commas = 0;
    
    for (i = 0; i < line_length; i++){
        if ( (is_white_space(line[i])) )
            continue;
        else
        {
            if (line[i] == ',')
                commas++;
            else
                commas = 0;
        }
        if (commas > 1)
            return 1;
    }
    return 0;
}


/**************************************************************************************/

/* The function gets a pointer to a string.
 It returns 0 - if not a number, 1 - for a number */

int is_number(char * string)
{
    int i;
    
    if (!(string[0] == '-' || string[0] =='+' || isdigit(string[0])))
        /* if doesn't start with a +, - or a digit */
        return 0;
    
    for (i = 1; string[i] != '\0'; i++)
        if (!isdigit(string[i]))
            /* if any of the other charachters is not a digit */
            return 0;
    
    return 1;
}

