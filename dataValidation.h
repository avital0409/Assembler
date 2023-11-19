/*
 Maman 14 > dataValidation.c
 Created by Avital on 17/03/2018
 */

#ifndef dataValidation_h
#define dataValidation_h

#define ERROR_DESCRIPTION_LENGTH 200

#include <stdio.h>

/* The function gets a pointer to a error description's string and line ID of the error.
 It turns the error flag on and print the error in a specified format */
void error_handler (char * error_description, int failedLine);

/* The function returns 0 if there were no errors found, and 1 if there are errors for the certain file */
int has_errors(void);

/* The function gets a pointer to a line struct and a string's poisition in the line.
 It validates if the string has apostrophes
 and returns 0 - for an invalid string (no apostrophes), 1 - for a valid string */
int validate_string (line *line, int wordPosition);

/* The function gets a pointer to a line struct the line's ID.
 It validates if there are no commas before or after a number, or double commas between 2 numbers
 and returns 0 - for an invalid line, 1 - for a valid line */
int validate_line(char *line, int lineID);

/* The function gets a pointer to a line struct.
 It validates if there are no commas before the first number,
 and returns 0 - for an invalid line, 1 - for a valid line*/
int has_comma_before_first_number(char *line);

/* The function gets a pointer to a line struct.
 It validates if there are no commas after the last number,
 and returns 0 - for an invalid line, 1 - for a valid line*/
int has_comma_after_last_number(char *line);

/* The function gets a pointer to a line struct.
 It validates if there are double commas between two numbers
 and returns 0 - for an invalid line, 1 - for a valid line*/
int has_commas_in_a_row(char * line, int line_length);

/* The function gets a pointer to a string.
 It returns 0 - if not a number, 1 - for a number */
int is_number(char * string);

#endif /* dataValidation_h */
