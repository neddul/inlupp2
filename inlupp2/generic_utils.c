#include "headers/generic_utils.h"

union answer
{
    int i;
    float f;
    char* s;
};


bool is_digit(char c)
{
    return('0' <= c && c <= '9'); 
}

bool is_number(char *str)
{
    int str_length = strlen(str);
    if(str[0]=='-' && str_length>1)
    {
        for(int i = 1; i < str_length; i++)
        {
            if(!(is_digit(str[i])))
            {   
                return false;
            }
        }
        return true;
    }
    else
    {   
        for(int i = 0; i < str_length; i++)
        {   
            
            if(!(is_digit(str[i])))
            {   
                return false;
            }
        }
        return true;
    }
}

bool not_empty(char *str)
{
    return strlen(str) > 0;
}

void clear_input_buffer()
{
    int c;
    do
    {
        c = getchar();
    }
    while(c != '\n' && c != EOF);
}

int read_string(char *buf, int buf_siz)
{
    int i = 0;
    for(; i < buf_siz-1; ++i)
    {
        int c;
        c = getchar();
        buf[i] = c;
        
        if(buf[i] == '\n' || buf[i] == EOF)
        {
            buf[i] = '\0';
            return i;
        }
    }
    clear_input_buffer();
    buf[i] = '\0';
    return i;
}

answer_t ask_question(char *question, check_func check, convert_func convert)
{
    int res = 0;
    int buf_siz = 255;
    char buf[buf_siz];
    
    do
    {
        printf("%s\n", question);
        res = read_string(buf, buf_siz);
        
    }while(res == 0 || !(check(buf))); //Denna körs tills att res inte är lika med noll, alltså inte en tom sträng och eller om den givna strängen inte är vad som förväntas.
    
    return convert(buf);
}

int ask_question_int(char *question)
{
    return ask_question(question, is_number, (convert_func) atoi).i;
}

char *ask_question_string(char *question)
{
    return ask_question(question, not_empty, (convert_func) strdup).s;
}


char *trim(char *str)
{
    char *start = str;                      //Pekar på index 0 i str
    char *end   = start + strlen(str)-1;    //Pekar på index n-1 i str
    
    while(isspace(*start))  ++start;        //Flyttar på start tills det inte längre finns "skräp"
    while(isspace(*end))      --end;        //-----------------------||---------------------------

    char *cursor = str;                     //Cursor pekar på str nu, som kan ha krympt
    for(; start <= end; ++start, ++cursor)  //När start ligger på samma adress som end, slutar for. låt start och cursor hoppa ett steg vid varje iteration
    {
        *cursor = *start;                   //Cursor pekar på str. Cursor skriver över värdet som ligger på "str[0]"" med det som ligger på "start[0]"
    }
    *cursor = '\0';                         //Slutligen läggs null till i slutet för att visa att strängen är slut
    
    return str;                             //Str som nu är ändrat skickas tillbaka
}
