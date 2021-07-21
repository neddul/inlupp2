#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>
#include "common.h"
#include "iterator.h"


struct iter
{
    link_t **current;
    ioopm_list_t *list;
};

ioopm_list_iterator_t *ioopm_list_iterator(ioopm_list_t *list)
{
    ioopm_list_iterator_t *new_iterator = calloc(1, sizeof(ioopm_list_iterator_t));
    new_iterator->current = &list->first;
    new_iterator->list = list;
    return new_iterator;
}

bool ioopm_iterator_has_next(ioopm_list_iterator_t *iter)
{
    bool res = ((*iter->current)->next != NULL);
    return res;
}

bool ioopm_iterator_next(ioopm_list_iterator_t *iter, elem_t *result)
{
    if(ioopm_iterator_has_next(iter))
    {
        iter->current = &((*iter->current)->next);
        *result = (*iter->current)->value; 
        return true;
    }
    else
    {
        return false;
    }
}

void ioopm_iterator_reset(ioopm_list_iterator_t **iter)
{
    (*iter)->current = &((*iter)->list->first);
}

bool ioopm_iterator_current(ioopm_list_iterator_t *iter, elem_t *result)
{
    if(*iter->current)
    {
        *result = (*iter->current)->value;
        return true;
    }
    return false;
}

void ioopm_iterator_destroy(ioopm_list_iterator_t **iter)
{
    free(*iter);
    *iter = NULL;
}
