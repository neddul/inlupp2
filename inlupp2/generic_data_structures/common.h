#pragma once

#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>


#define int_elem(x)     (elem_t) { .int_val    = (x)    }
#define ptr_elem(x)     (elem_t) { .ptr_val    = (x)    }
#define str_elem(x)     (elem_t) { .str_val    = (x)    }
#define unint_elem(x)   (elem_t) { .uint_val   = (x)    }
#define float_elem(x)   (elem_t) { .float_val  = (x)    }

typedef struct link link_t;
typedef union elem elem_t;
typedef bool(*ioopm_eq_function)(elem_t a, elem_t b);

union elem
{
  int int_val;
  unsigned int uint_val;
  bool bool_val;
  float float_val;
  char *str_val;
  void *ptr_val;
  unsigned long int ulint_val;
};

struct link
{
    elem_t value;
    link_t *next;
};

struct list
{
    size_t size;
    link_t *first;
    link_t *last;
    ioopm_eq_function eq_function;                                  
};
