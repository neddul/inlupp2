#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include "linked_list.h"

static link_t *link_create(elem_t value, link_t *next);

ioopm_list_t *ioopm_linked_list_create(ioopm_eq_function eq)       
{
    ioopm_list_t *linked_list = calloc(1, sizeof(ioopm_list_t));
    linked_list->eq_function = eq;                                           
    return linked_list;
}

void ioopm_linked_list_destroy(ioopm_list_t *list)
{
    ioopm_linked_list_clear(list);
    free(list);
}

void ioopm_linked_list_append(ioopm_list_t *list, elem_t value)
{
    link_t *new_link = link_create(value, NULL);
    
    if (ioopm_linked_list_is_empty(list))
    {
        list->first = new_link;
        list->last = new_link;
    }
    else
    {
        list->last->next = new_link;
        list->last = new_link;
    }
    list->size += 1;
}

void ioopm_linked_list_prepend(ioopm_list_t *list, elem_t value)
{
    link_t *new_link = link_create(value, NULL);
    
    if (ioopm_linked_list_is_empty(list))
    {
        list->first = new_link;
        list->last = new_link;
    }
    else
    {
        new_link->next = list->first;
        list->first = new_link;
    }
    list->size += 1;
}

size_t ioopm_linked_list_size(ioopm_list_t *list)
{
    return list->size;
}

bool ioopm_linked_list_is_empty(ioopm_list_t *list)
{
    return 0 == ioopm_linked_list_size(list);
}

void ioopm_linked_list_clear(ioopm_list_t *list)
{
    size_t list_size = ioopm_linked_list_size(list);
    elem_t res_ignored;
    
    while(list_size > 0)
    {
        ioopm_linked_list_remove(list, 0, &res_ignored);
        --list_size;
    }
}

bool ioopm_linked_list_remove(ioopm_list_t *list, int index, elem_t *value)
{
    int list_size = ioopm_linked_list_size(list);
    if (index > list_size - 1)
    {
        errno = EINVAL;
        return false;
    }
    
    link_t *cursor = list->first;
    if(index == 0)
    {
        list->first = cursor->next;
        *value = cursor->value;
        free(cursor);
        
        list->size -= 1;
        return true;    
    }
    else if(index == list_size-1)
    {
        while(cursor->next->next)
        {
            cursor = cursor->next;
        }
        list->last = cursor;
        link_t *tmp = cursor->next;
        cursor->next = NULL;
        cursor = tmp;
        *value = cursor->value;
        free(cursor);
        
        list->size -= 1;
        return true;
    }
    else
    {
        for(int i = 0; i < index - 1; ++i)
        {
            cursor = cursor->next;          //Stannar innan linken man vill ta bort
        }
        link_t *tmp = cursor->next;         
        cursor->next = cursor->next->next;  //Pekaren innan det man vill ta bort byts till den det man vill ta bort på.
        cursor = tmp;
    
        *value = cursor->value;
        free(cursor);
        
        list->size -=1;
        return true;
    }
}

bool ioopm_linked_list_insert(ioopm_list_t *list, int index, elem_t value)
{
    int list_size = ioopm_linked_list_size(list);
    
    if(list_size<index)
    {
        errno = EINVAL;
        return false;
    }
    
    if(index == 0)
    {
        ioopm_linked_list_prepend(list, value);
        return true;
    }
    else if(index == list_size-1)
    {
        ioopm_linked_list_append(list, value);
        return true;
    }
    else
    {
        link_t *cursor = list->first;
        
        for(int i = 0; i < index - 1; ++i)
        {
            cursor = cursor->next;          //Stannar innan linken där man vill lägga till något
        }
        
        link_t *new_link = link_create(value, cursor->next);
        cursor->next = new_link; 
    
        list->size += 1;
        return true;
    }
}

elem_t ioopm_linked_list_get(ioopm_list_t *list, int index)
{
    link_t *cursor = list->first;   
    for(int i = 0; i < index; ++i)
    {
        cursor = cursor->next;
    }
    return cursor->value;
    
}

bool ioopm_linked_list_contains(ioopm_list_t *list, elem_t value)
{
    link_t *cursor = list->first;
    while(cursor->next)
    {
        if(list->eq_function(cursor->value, value))
        {
            return true;
        }
        cursor = cursor->next;
    }
    return false;
}

bool ioopm_linked_list_all(ioopm_list_t *list, ioopm_char_predicate prop, void *extra)
{
    link_t *cursor = list->first;
    while(cursor)
    {
        if(!prop(cursor->value, extra))
        {
            return false;
        }
        cursor = cursor->next;
    }
    return true;
}

bool ioopm_linked_list_any(ioopm_list_t *list, ioopm_char_predicate prop, void *extra)
{
    link_t *cursor = list->first;
    while(cursor)
    {
        if(prop(cursor->value, extra))
        {
            return true;
        }
        cursor = cursor->next;
    }
    return false;
}

void ioopm_linked_apply_to_all (ioopm_list_t *list, ioopm_apply_char_function fun, void *extra)
{
  link_t *prev_link = list->first;
  link_t *next_link = prev_link;
  while (next_link != NULL)
    {
      fun (&next_link->value, extra);
      next_link = prev_link->next;
      prev_link = next_link;
    }
  return;
}

//
// ** PRIVATE FUNCTIONS **
//
link_t *link_create(elem_t value, link_t *next)
{
    link_t *new_link = calloc(1, sizeof(link_t));
    new_link->value = value;
    new_link->next = next;
    return new_link;
}
