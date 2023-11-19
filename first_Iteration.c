/*
 Maman 14 > first_Iteration.c
 Created by Avital Rubichi on 10/03/2018.
 */

#include <string.h>
#include <stdlib.h>
#include "dataStructure.h"
#include "dataValidation.h"
#include "signs.h"
#include "utils.h"
#include "first_Iteration.h"

static int linesCount = 0;
static int decimalAddress = START_ADDRESS;

extern struct assembler_output *codeTable;
extern struct assembler_output *dataTable;
extern struct label *labels;
extern struct label *usedLabels;
extern struct label *entryLabels;
extern struct label *declaredExternalLabels;


void first_pass_on_line(struct line *current_line, char *wholeLine);

void handle_instruction_line(struct line *line, char * wholeLine);
void handle_command_line(struct line *line, char *command);
void handle_data_numbers (struct line *line, char *wholeLine);
void handle_string(struct line *line);
void handle_struct(struct line * line);
void handle_entry_extern(struct line *line, int instructionCode);
void handle_label_declaration(struct line *line, struct assembler_output *ao_line);
void handle_operands(struct line *line, int has_label, int operands2expect, int source_type, int dest_type);
int how_many_operands2expect(int commandCode);
void handle_operand_as_number(struct word *word, int lineID);
void handle_operand_as_label(char *word, int lineID);
void handle_operand_as_struct(char *word, int lineID);
void handle_operand_as_register(struct line *line, int position, int source_type, int dest_type);
void handle_fieldID(char *fieldID, int lineID);
void keep_used_label(char *word, int decimalAddress, int lineID);



/**************************************************************************************/
/* The function gets a file.
 It parses every line in the file to line struct, filled with word structs and some other data,
 and then performing first iteration on the line */

void prepare_for_first_iteration(FILE *file)
{
    
    char lineRead[LINE_SIZE]; /*get line from file into here*/
    
    
    while (fgets(lineRead, sizeof(lineRead), file))
    { /*read lines until the end of the file*/
        if (should_skip_line(lineRead))
        {
            int j;
            struct line *current_line = new_line;
            
            linesCount++;
            
            * current_line = (struct line) {
                .word = (struct word *) malloc(10*sizeof(struct word)*LINE_SIZE) ,
                .lineID = linesCount ,
                .wordsCount = 0 ,
                .hasLabel=0};
            
            for (j=0; j<LINE_SIZE; j++)
            {
                struct word *current_word = new_word;
                memset(current_word->string, 0, LINE_SIZE);
                current_word->position = 0;
                current_word->type = UNKNOWN;
                current_word->length = 0;
                current_line->word[j] = current_word;
                
            }
            line_to_words(current_line,lineRead);
            first_pass_on_line(current_line, lineRead);
        }
    }
}

/**************************************************************************************/
/* The function gets a pointer to line struct and a pointer to the line's string.
 It categorizes the line to command / instruction type, and calls the related handler function accordingly */

void first_pass_on_line(struct line *current_line, char *wholeLine)
{
    int first_word_position;
    
    first_word_position = current_line->hasLabel;
    
    /* if the first word is a label, we adjust its' length to ignore the expected ':' in the end */
    
    analyze_word(current_line->word[first_word_position]); /* puts type of first word that is not lablel + its position into the wordToHandle */
    current_line->word[first_word_position]->position = current_line->hasLabel;
    
    if (current_line->word[first_word_position]->type == INSTRUCTION)
        handle_instruction_line(current_line, wholeLine);
    
    else
        handle_command_line(current_line, current_line->word[first_word_position]->string);
    
}

/**************************************************************************************/
/* The function gets a pointer to line struct (assuming it is an instruction line) and a pointer to the line's string.
 It finds the instruction code for the line, and calls relevant handler accordingly */

void handle_instruction_line(struct line *line, char * wholeLine)
{
    int coded_instruction;
    
    coded_instruction = get_instruction_code(line->word[line->hasLabel]);
    
    switch (coded_instruction)
    {
        case DATA:
        {
            handle_data_numbers(line, wholeLine);
            break;
        }
        case STRING:
        {
            handle_string(line);
            break;
        }
        case STRUCT:
        {
            handle_struct(line);
            break;
        }
        case ENTRY:
        case EXTERN:
        {
            handle_entry_extern(line, coded_instruction);
            break;
        }
    }
}

/**************************************************************************************/
/* The function gets a pointer to line struct and a pointer to relevant assembler output.
 If the line declares a label, it checks for label validity and if the label is valid it
 is added to the declared labels list and adds the label name to ao_line */

void handle_label_declaration(struct line *line, struct assembler_output *ao_line)
{
    if (line->hasLabel)
    {
        switch (keepLabel(line->word[0],ao_line->dAddress))
        {
            case -1:
            {
                char failure[ERROR_DESCRIPTION_LENGTH];
                sprintf(failure,"Line %d defines the label \"%s\" that has been defined before", line->lineID, line->word[0]->string);
                error_handler(failure,line->lineID);
                return;
            }
            case -2:
            {
                char failure[ERROR_DESCRIPTION_LENGTH];
                sprintf(failure,"Line %d contains an invalid label \"%s\" (using reserved word)", line->lineID, line->word[0]->string);
                error_handler(failure,line->lineID);
                return;
            }
            case -3:
            {
                char failure[ERROR_DESCRIPTION_LENGTH];
                sprintf(failure,"Line %d contains a label \"%s\" that is longer than %d characters", line->lineID, line->word[0]->string, LABEL_SIZE);
                error_handler(failure,line->lineID);
                return;
            }
            default:
            {
                strcpy(ao_line->label, line->word[0]->string);
                break;
            }
        }
    }
}

/**************************************************************************************/
/* The function gets a pointer to line struct (assuming it is a command line) and a pointer to the line's string.
 It finds the code and expected number of operands for the line, and fills new nodes in the codeTable */

void handle_command_line(struct line *line, char *command)
{
    int * converted_value = (int *) malloc(sizeof(int)*BITS_6to9_SIZE*10);
    int index;
    int dest_type = -1;;
    int operands2expect;
    int source_type = -1;
    int command_code = -1;
    int binaryCode[MILA_SIZE] = {0};
    struct assembler_output *ao_line = new_ao_line;
    
    * ao_line = (struct assembler_output) {
        .label = {0} ,
        .dAddress = 0 ,
        .base32Address = new_base32Address ,
        .binaryCode = {0} ,
        .base32Code = {0} ,
        .is_using_label = 0 ,
        .used_label = {0} ,
        .origin_lineID = line->lineID ,
        .next = new_ao_line};
    
    ao_line->dAddress = decimalAddress++;
    
    translate_from_base_10(ao_line->dAddress, WEIRD_BASE, ao_line->base32Address, 0, 1, WEIRD_BASE_32_SIZE);
    
    handle_label_declaration(line, ao_line);
    
    /* handle operation code bits */
    index = line->hasLabel;
    command_code = get_command_code(line->word[index]->string);
    
    memset(converted_value,0,sizeof(int)*BITS_6to9_SIZE*10);
    
    translate_from_base_10(command_code,BINARY_BASE,converted_value,0, 1, BITS_6to9_SIZE);
    put_binary_code_in_place(converted_value, 6, 9, binaryCode);
    
    /* handle bits destination code bits */
    operands2expect = how_many_operands2expect(command_code);
    
    if (operands2expect == -1)
    {
        char failure[ERROR_DESCRIPTION_LENGTH];
        sprintf(failure,"%s is an illegal operand", line->word[index]->string);
        error_handler(failure,line->lineID);
    }
    
    else
    {
        int dest_type_converted [BITS_0to5_SIZE];
        
        if (operands2expect > 0)
        {
            dest_type = get_addressing_type(line->word[index + operands2expect]);
            translate_from_base_10(dest_type,BINARY_BASE,dest_type_converted, 0, 1, BITS_0to5_SIZE);
            put_binary_code_in_place(dest_type_converted, 2, 3, binaryCode);
            
            if (operands2expect == 2)
            {
                /*there should be source destination code */
                int source_type_converted[BITS_0to5_SIZE];
                
                source_type = get_addressing_type(line->word[index+1]);
                translate_from_base_10(source_type,BINARY_BASE, source_type_converted, 0, 1, BITS_0to5_SIZE);
                put_binary_code_in_place(source_type_converted, 4, 5, binaryCode);
            }
        }
        
        memcpy(ao_line->binaryCode, &binaryCode, MILA_SIZE * sizeof(int));
        add_NodeTo_codeTable(ao_line);
        
        handle_operands(line, index, operands2expect, source_type, dest_type);
    }
}

/**************************************************************************************/
/* The function gets a pointer to line struct (assuming it is a .data instruction line) and a pointer to the line's string.
 It creates a new node in the dataTable for every number, and checks commas validity */

void handle_data_numbers (struct line *line, char * wholeLine)
{
    int position;
    int i;
    
    position = line->hasLabel+1;
    
    /* loop throw all words in line and save its information in the data list*/
    for (i = position; i < line->wordsCount; i++)
    {
        int is_negative = 0;
        int integer = 0;
        char tmp[MILA_SIZE] = {0};
        int j;
        
        
        struct assembler_output *ao_line = new_ao_line;
        
        * ao_line = (struct assembler_output) {
            .label = {0} ,
            .dAddress = 0 ,
            .base32Address = new_base32Address ,
            .binaryCode = {0} ,
            .base32Code = {0} ,
            .is_using_label = 0 ,
            .used_label = {0} ,
            .origin_lineID = line->lineID ,
            .next = new_ao_line};

        ao_line->dAddress = decimalAddress++;
        
        if (i == position)
        /* if it is the first word, create appropriate label if needed to */
            handle_label_declaration(line, ao_line);
        
        translate_from_base_10(ao_line->dAddress, WEIRD_BASE, ao_line->base32Address, 0, 0, WEIRD_BASE_32_SIZE);
        
        
        integer = atoi(tmp);
        if (line->word[i]->string[0] == '-')
            is_negative = 1;

            for (j = 0; j < line->word[i]->length ; j++)
                strncpy(&tmp[j],&line->word[i]->string[j],sizeof(char));
        
        integer = atoi(tmp);
        
        if (is_negative)
            integer *= (-1);
        
        translate_from_base_10(integer, BINARY_BASE, ao_line->binaryCode, is_negative, 0, MILA_SIZE);
        
        add_NodeTo_dataTable(ao_line);
    }

}

/**************************************************************************************/
/* The function gets a pointer to line struct (assuming it is a .string instruction line).
 It creates a new node in the dataTable for every letter in the string */

void handle_string(struct line *line)
{
    int position;
    int i;
    char string[LINE_SIZE] = {0};

    position = line->hasLabel +1;
    
    /* remove quotes from string */
    for (i=1; i < (line->word[position]->length-1); i++)
        strncpy(&string[i-1],&line->word[position]->string[i],1);
    
    string[i]='0';
    
    for (i = 0; i < line->word[position]->length-1; i++)
    {
        struct assembler_output *character_line = new_ao_line;
        * character_line = (struct assembler_output) {
            .label = {0} ,
            .dAddress = 0 ,
            .base32Address = new_base32Address ,
            .binaryCode = {0} ,
            .base32Code = {0} ,
            .is_using_label = 0 ,
            .used_label = {0} ,
            .origin_lineID = line->lineID ,
            .next = new_ao_line};

        character_line->dAddress = decimalAddress++;
        
        if (i == 0 && line->hasLabel)
            handle_label_declaration(line, character_line);
        
        translate_from_base_10(string[i], BINARY_BASE, character_line->binaryCode, 0, 0, MILA_SIZE);
        translate_from_base_10(character_line->dAddress,WEIRD_BASE,character_line->base32Address, 0, 1, WEIRD_BASE_32_SIZE);
        
        add_NodeTo_dataTable(character_line);
    }
}

/**************************************************************************************/
/* The function gets a pointer to line struct (assuming it is a .struct instruction line).
 It creates a new node in the dataTable for the struct addressed field, and another node for the number */

void handle_struct(struct line * line)
{
    char string[LINE_SIZE];
    int is_negative = 0;
    int integer = 0;
    char *tmp = NULL;
    int position;
    int i;
    
    
    /* Create the first table line for the first word */
    struct assembler_output *ao_line_number = new_ao_line;
    
    * ao_line_number = (struct assembler_output) {
        .label = {0} ,
        .dAddress = 0 ,
        .base32Address = new_base32Address ,
        .binaryCode = {0} ,
        .base32Code = {0} ,
        .is_using_label = 0 ,
        .used_label = {0} ,
        .origin_lineID = line->lineID ,
        .next = new_ao_line};

    ao_line_number->dAddress = decimalAddress++;
    
    if (line->hasLabel)
        handle_label_declaration(line, ao_line_number);
    
    position = line->hasLabel +1;
    
    /* handle first param = number */
    translate_from_base_10(ao_line_number->dAddress, WEIRD_BASE, ao_line_number->base32Address, 0, 1, WEIRD_BASE_32_SIZE);
    
    /* Get the index for the first word that isnt a label*/
    
    if (line->word[position]->string[0] == '-')
    {
        tmp = &line->word[position]->string[1];
        is_negative = 1;
    }
    else
        tmp = &line->word[position]->string[0];
    
    integer = atoi(tmp);
    
    if (is_negative)
        integer *= (-1);
    
    translate_from_base_10(integer, BINARY_BASE, ao_line_number->binaryCode, is_negative, 0, MILA_SIZE);
    
    add_NodeTo_dataTable(ao_line_number);
    
    position++;
    
    /* handle second param = string */
    for (i = 1; i < line->word[position]->length-1; i++)
        string[i-1] = line->word[position]->string[i];
    string[i]='0';
    
    for (i = 0; i < line->word[position]->length-1; i++)
    {
        struct assembler_output *ao_line_string = new_ao_line;
        
        * ao_line_string = (struct assembler_output) {
            .label = {0} ,
            .dAddress = 0 ,
            .base32Address = new_base32Address ,
            .binaryCode = {0} ,
            .base32Code = {0} ,
            .is_using_label = 0 ,
            .used_label = {0} ,
            .origin_lineID = line->lineID ,
            .next = new_ao_line};

        ao_line_string->dAddress = decimalAddress++;
        
        translate_from_base_10(string[i], BINARY_BASE, ao_line_string->binaryCode, 0, 0, MILA_SIZE);
        translate_from_base_10(ao_line_string->dAddress,WEIRD_BASE, ao_line_string->base32Address, 0, 1, WEIRD_BASE_32_SIZE);
        
        add_NodeTo_dataTable(ao_line_string);
    }
}

/**************************************************************************************/
/* The function gets a pointer to line struct (assuming it is a .entry or .extern instruction line) and its' instruction code.
 It creates a new node in the entryLabels or externalLabels accordingly */

void handle_entry_extern(struct line *line, int instructionCode)
{
    int position = line->hasLabel+1;
    int i;
    
    /* Loop throw all words in line*/
    for (i = position; i < line->wordsCount; i++)
    {
        struct label *newLabel = new_label;
        
        * newLabel = (struct label) {
            .string = {0} ,
            .dAddress = 0 ,
            .base32Address = new_base32Address ,
            .using_line = 0 ,
            .type = REGULAR ,
            .next = new_label};

        strcpy(newLabel->string, line->word[i]->string);
        
        switch (instructionCode)
        {
            case ENTRY:
            {
                newLabel->type = ENT;
                add_NodeTo_entryLabels(newLabel);
                break;
            }
            case EXTERN:
            {
                newLabel->type = EXT;
                newLabel->using_line = line->lineID;
                add_NodeTo_externalLabels(newLabel);
                break;
            }
        }
    }
}

/**************************************************************************************/
/* The function gets a command ID and returns how many operands to expect, in the range of 0 to 2 */

int how_many_operands2expect(int commandID)
{
    switch (commandID)
    {
        case -1:
        case RTS:
        case STOP:
            return 0;
        case NOT:
        case CLR:
        case INC:
        case DEC:
        case JMP:
        case BNE:
        case RED:
        case PRN:
        case JSR:
            return 1;
        case MOV:
        case CMP:
        case ADD:
        case SUB:
        case LEA:
            return 2;
    }
    return 0;
}

/**************************************************************************************/
/* The function gets a pointer to line struct, an inidcator for declaration of a label in the line,
 how many operands to expect and the line's source and destination types.
 It calls relevant handlers for each operand */

void handle_operands(struct line *line, int has_label, int operands2expect, int source_type, int dest_type)
{
    int handled_register = 0;
    
    if (operands2expect > 0)
    {
        switch (source_type)
        {
            case -1:
                break;
            case IMMEDIATE:
            {
                handle_operand_as_number(line->word[has_label + 1], line->lineID);
                break;
            }
            case DIRECT:
            {
                handle_operand_as_label(line->word[has_label + 1]->string,line->lineID);
                break;
            }
            case STRUCT_ACCESS:
            {
                handle_operand_as_struct(line->word[has_label + 1]->string, line->lineID);
                break;
            }
            case DIRECT_REGISTER:
            {
                handle_operand_as_register(line, has_label, source_type, dest_type);
                handled_register = 1;
                break;
            }
                
        }
        
        switch (dest_type)
        {
            case -1:
                break;
            case IMMEDIATE:
            {
                handle_operand_as_number(line->word[has_label + operands2expect], line->lineID);
                break;
            }
            case DIRECT:
            {
                handle_operand_as_label(line->word[has_label + operands2expect]->string,line->lineID);
                break;
            }
            case STRUCT_ACCESS:
            {
                handle_operand_as_struct(line->word[has_label + operands2expect]->string, line->lineID);
                break;
            }
            case DIRECT_REGISTER:
            {
                if (!handled_register)
                    handle_operand_as_register(line, has_label, source_type, dest_type);
                break;
            }
        }
    }
}

/**************************************************************************************/
/* The function gets a pointer to word struct (assuming it is a number) and its line ID.
 It adds a node in codeTable for the number operand */

void handle_operand_as_number(struct word *word, int lineID)
{
    char tmp[MILA_SIZE] = {0};
    int integer = 0;
    int is_negative = 0;
    int binaryCode[MILA_SIZE-ERA_SIZE] = {0};
    
    memset(binaryCode,0,MILA_SIZE-ERA_SIZE*sizeof(int));
    
    struct assembler_output *ao_line = new_ao_line;
    
    * ao_line = (struct assembler_output) {
        .label = {0} ,
        .dAddress = 0 ,
        .base32Address = new_base32Address ,
        .binaryCode = {0} ,
        .base32Code = {0} ,
        .is_using_label = 0 ,
        .used_label = {0} ,
        .origin_lineID = lineID ,
        .next = new_ao_line};
    
    ao_line->dAddress = decimalAddress++;
    
    /*translate the decimal address of the line to 32 base*/
    translate_from_base_10(ao_line->dAddress, WEIRD_BASE, ao_line->base32Address, 0, 1, WEIRD_BASE_32_SIZE);

    if (word->string[1] == '-')
    {
        memcpy(tmp, &word->string[2], word->length - 2);
        is_negative = 1;
    }
    else
        memcpy(tmp, &word->string[1], word->length - 1);
    
    integer = atoi(tmp);
    
    translate_from_base_10(integer, BINARY_BASE, binaryCode, is_negative, 1, MILA_SIZE-ERA_SIZE);
    put_binary_code_in_place(binaryCode, ERA_SIZE, MILA_SIZE-1, ao_line->binaryCode);
    
    add_NodeTo_codeTable(ao_line);
}

/**************************************************************************************/
/* The function gets a pointer to word struct (assuming it is a label) and its lineID.
 It adds a node in codeTable for the label operand and also add a node to the usedLabels */

void handle_operand_as_label(char *word, int lineID)
{
    struct assembler_output *ao_line = new_ao_line;
    
    * ao_line = (struct assembler_output) {
        .label = {0} ,
        .dAddress = 0 ,
        .base32Address = new_base32Address ,
        .binaryCode = {0} ,
        .base32Code = {0} ,
        .is_using_label = 0 ,
        .used_label = {0} ,
        .origin_lineID = lineID ,
        .next = new_ao_line};

    ao_line->dAddress = decimalAddress++;
    ao_line->is_using_label = 1;
    
    translate_from_base_10(ao_line->dAddress, WEIRD_BASE, ao_line->base32Address, 0, 1, WEIRD_BASE_32_SIZE);
    
    strcpy(ao_line->used_label,word);
    
    /* Add to used sign table */
    keep_used_label(ao_line->used_label, ao_line->dAddress, lineID);
    add_NodeTo_codeTable(ao_line);
}

/**************************************************************************************/
/* The function gets a pointer to word struct (assuming it is a struct), and its lineID.
 It adds a node in codeTable for the struct's label, and a node for the fieldID */

void handle_operand_as_struct(char *word, int lineID)
{
    char *tmp;
    
    /*get address as a string (label)*/
    tmp = strtok(word, ".");
    
    handle_operand_as_label(tmp, lineID);
    
    /*get field_ID*/
    tmp=strtok(NULL, "\0");
    
    if ((strcmp(tmp,"1")!=0 && strcmp(tmp,"2")!=0) || strlen(tmp)>1)
    {
        char failure[ERROR_DESCRIPTION_LENGTH];
        sprintf(failure,"Illegal struct field ID was entered");
        error_handler(failure,lineID);
    }
    
    handle_fieldID(tmp, lineID);
    
}

/**************************************************************************************/
/* The function gets a pointer to word struct (assuming it is a register).
 It adds a node in codeTable for the register number */

void handle_operand_as_register(struct line *line, int position, int source_type, int dest_type)
{
    int combinedBinaryCode[MILA_SIZE] = {0};
    int base32address[WEIRD_BASE_32_SIZE];
    
    struct assembler_output *ao_line = new_ao_line;
    
    * ao_line = (struct assembler_output) {
        .label = {0} ,
        .dAddress = 0 ,
        .base32Address = new_base32Address ,
        .binaryCode = {0} ,
        .base32Code = {0} ,
        .is_using_label = 0 ,
        .used_label = {0} ,
        .origin_lineID = line->lineID ,
        .next = new_ao_line};

    ao_line->dAddress = decimalAddress++;
    translate_from_base_10(ao_line->dAddress, WEIRD_BASE, base32address, 0, 1, WEIRD_BASE_32_SIZE);
    memcpy(ao_line->base32Address, base32address, WEIRD_BASE_32_SIZE * sizeof(int));
    
    if ((source_type == 3) && is_register(line->word[position + 1]))
    {
        int source_reg_converted[REGISTER_CODED_SIZE] = {0};
        int source_reg_number;
        
        source_reg_number = atoi(&line->word[position+1]->string[1]);
        translate_from_base_10(source_reg_number, BINARY_BASE, source_reg_converted,0, 1, REGISTER_CODED_SIZE);
        put_binary_code_in_place(source_reg_converted, SOURCE_REGISTER, MILA_SIZE-1, combinedBinaryCode);
    }
    
    if ((dest_type == 3) && is_register(line->word[position + 2]))
    {
        int dest_reg_converted[REGISTER_CODED_SIZE] = {0};
        int dest_reg_number;
        
        dest_reg_number = atoi(&line->word[position+2]->string[1]);
        translate_from_base_10(dest_reg_number, BINARY_BASE, dest_reg_converted,0, 1, REGISTER_CODED_SIZE);
        put_binary_code_in_place(dest_reg_converted, DEST_REGISTER, SOURCE_REGISTER-1, combinedBinaryCode);
    }
    
    memcpy(ao_line->binaryCode, combinedBinaryCode, MILA_SIZE * sizeof(int));
    add_NodeTo_codeTable(ao_line);
}

/**************************************************************************************/
/* The function gets a pointer to a string representing struct fieldID (assuming it has the value 1 or 2) and its line ID.
 It adds a node in codeTable for the field number */

void handle_fieldID(char *fieldID, int lineID)
{
    
    int integer = 0;
    
    struct assembler_output *ao_line = new_ao_line;
    int binaryCode[MILA_SIZE-ERA_SIZE] = {0};
    
    * ao_line = (struct assembler_output) {
        .label = {0} ,
        .dAddress = 0 ,
        .base32Address = new_base32Address ,
        .binaryCode = {0} ,
        .base32Code = {0} ,
        .is_using_label = 0 ,
        .used_label = {0} ,
        .origin_lineID = lineID ,
        .next = new_ao_line};

    ao_line->dAddress = decimalAddress++;
    
    /*translate the decimal address of the line to 32 base*/
    translate_from_base_10(ao_line->dAddress, WEIRD_BASE, ao_line->base32Address, 0, 1, WEIRD_BASE_32_SIZE);
    
    integer = atoi(fieldID);
    
    translate_from_base_10(integer, BINARY_BASE, binaryCode, 0, 1, MILA_SIZE-ERA_SIZE);
    put_binary_code_in_place(binaryCode, 2, MILA_SIZE-1, ao_line->binaryCode);
    
    
    add_NodeTo_codeTable(ao_line);
}

/**************************************************************************************/
/* The function gets a pointer to a string representing a label name, its decimal address and line ID.
 It adds the label as a new node in usedLabels */

void keep_used_label(char *labelName, int decimalAddress, int lineID)
{
    struct label *used = new_label;
    * used = (struct label) {
        .string = {0} ,
        .dAddress = decimalAddress ,
        .base32Address = new_base32Address ,
        .using_line = lineID ,
        .type = REGULAR ,
        .next = new_label};

    strcpy(used->string, labelName);
    translate_from_base_10(decimalAddress, WEIRD_BASE, used->base32Address, 0, 1, WEIRD_BASE_32_SIZE);
    
    add_NodeTo_usedLabels(used);
}

