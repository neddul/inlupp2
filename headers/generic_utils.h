#pragma once
#include <stdbool.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>


typedef union answer answer_t;

typedef bool(*check_func)(char*);
typedef answer_t(*convert_func)(char*);

extern char *strdup(const char *);


///-----------------------------------------------------------------------------------------------------------------------------------------------------------------
///-------------------------------------------------------------------------Check functions-------------------------------------------------------------------------
///-----------------------------------------------------------------------------------------------------------------------------------------------------------------


///@brief checks if a given char c is a digit
///@param character c operated upon
///@returns true if c is a digit, else false
bool is_digit(char c);


///@brief checks if a given string is an integer or not
///@param the given char* str is operated upon
///@returns true if str is an integer, else false
bool is_number(char *str);


///@brief checks if a given string is empty or not
///@param a string to operate on
///@returns true if str is not empty, else false
bool not_empty(char *str);


///-----------------------------------------------------------------------------------------------------------------------------------------------------------------
///-------------------------------------------------------------------------Misc------------------------------------------------------------------------------------
///-----------------------------------------------------------------------------------------------------------------------------------------------------------------


///@brief clears the input buffer
void clear_input_buffer();

///@brief removes everything at the start and the end of a string that isn't a-z or A-Z
///@param a string str that is subject to possibly trimmed
///@returns the given string str but without the "trash" at the start and end of str 
char *trim(char *str);


///-----------------------------------------------------------------------------------------------------------------------------------------------------------------
///-------------------------------------------------------------------------Help-func-------------------------------------------------------------------------------
///-----------------------------------------------------------------------------------------------------------------------------------------------------------------


///@brief "reads" a string and calculates the length of the given string
///@param buf the string given by the user
///@param the biggest allowed size of buf
///@returns the amount of characters in the given string as an integer
int read_string(char *buf, int buf_siz);


///-----------------------------------------------------------------------------------------------------------------------------------------------------------------
///-------------------------------------------------------------------------Ask-question----------------------------------------------------------------------------
///-----------------------------------------------------------------------------------------------------------------------------------------------------------------


///@brief   asks a generic question, gets an input from keyboard, checks if input is as expected with check, converts input
///@param   question that is prompted to user
///@param   a function check that returns a bool, to see wether input is as expected
///@param   a function convert that converts input from user
///@returns a union answer_t that will be determined later
answer_t ask_question(char *question, check_func check, convert_func convert);

///@brief   asks a question untill an integer is inserted by the user
///@param   asks a given question
///@returns an integer
int ask_question_int(char *question);

///@brief   asks a question untill a non-empty string is inserted by the user
///@param   asks a given question
///@returns a non-empty string
char *ask_question_string(char *question);