/*
 Maman 14 > utils.h
 Created by Avital on 08/03/2018
 */


#ifndef utils_h
#define utils_h

#include <stdio.h>

#define FILE_SIZE_BUFFER 1024
#define WORD_SIZE 256
#define LINE_SIZE 80
#define LABEL_SIZE 30
#define MILA_SIZE 10
#define START_ADDRESS 100
#define WEIRD_BASE_32_SIZE 2
#define WEIRD_BASE 32
#define BINARY_BASE 2
#define MAX_VALUE 170
#define BITS_6to9_SIZE 4
#define BITS_0to5_SIZE 2
#define ERA_SIZE 2
#define SOURCE_REGISTER 6
#define DEST_REGISTER 2
#define REGISTER_CODED_SIZE 4

/**************************************************************************************/
/* The function gets a pointer to string representing a whole line and checks if the line should be skipped.
 It returns 1 - for skip, 0 - not skip */
int should_skip_line(char *line);

/* The function gets a char and tells if it is a white space (space or tab)
 It returns 1 - for white space, 0 - not white space */
int is_white_space(char character);

/* The function gets a pointer to a string representing label name and an int representing if the questioned label
 is asked as a declared label or not. It returns 1 - for label, 0 - not label */
int is_label(char *word, int is_declaration);

/* The function replaces source binary code in binary output, the range of the bits to put the code in (from - to) */
void put_binary_code_in_place(int *source, int from_index, int to_index, int * binary_output);

/* The function converts to "convert_to_base" from decimal base. */
void translate_from_base_10 (int value_to_convert, int convert_to_base, int * converted_value, int is_negative, int needs_flipping, int num_of_representation_digits);

/* The function converts from "convert_to_base" to decimal base. */
void translate_to_base_10(int *value_to_convert, int convert_from_base, int *converted_value, int length);

/* The function converts base 32 to weird base 32 */
void get_weird_characters(int *decimal, int length, char *converted_to_weird);

/* The function converts base 32 address and machine code in 32 base to weird adderss, and returns weird output that has them*/
struct weird_output *translate_to_weird_language(int *base32Address , int *binary32Code);

#endif /* utils_h */

