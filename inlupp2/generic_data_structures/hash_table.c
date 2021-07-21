#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <errno.h>
#include "hash_table.h"

#define Default_no_buckets 17
#define Default_load_factor 14.0        


typedef struct entry entry_t;


struct entry
{
    elem_t key;          // holds the key
    elem_t value;        // holds the value
    entry_t *next;       // points to the next entry (possibly NULL)
};

struct hash_table
{
    entry_t *buckets;
    ioopm_eq_function key_eq_function;
    ioopm_eq_function value_eq_function;
    ioopm_hash_function hash_function;
    size_t size;
    size_t no_buckets;
    float load_factor;
};

static entry_t *entry_create(ioopm_hash_table_t *ht, elem_t key, elem_t value, entry_t *next);
static void entry_destroy(entry_t **entry_to_destroy);
static void entries_destroy_all_iterativ(entry_t **e);
static bool key_equiv(ioopm_hash_table_t *ht, elem_t key, elem_t value_ignored, void *arg);
static bool val_equiv(ioopm_hash_table_t *ht, elem_t key_ignored, elem_t value, void *arg);
static ioopm_hash_table_t *hash_table_create_custom(ioopm_eq_function key_eq, 
                                                    ioopm_eq_function val_eq, 
                                                    ioopm_hash_function hash_function,
                                                    size_t buckets,
                                                    float load);
                                                    
static bool key_equiv(ioopm_hash_table_t *ht, elem_t key, elem_t value_ignored, void *arg)
{
  elem_t other_key = *(elem_t *) arg;
  return ht->key_eq_function(key, other_key);
}

static bool val_equiv(ioopm_hash_table_t *ht, elem_t key_ignored, elem_t value, void *arg)
{
  elem_t *tmp = arg;
  elem_t val_compare = *tmp;
  return ht->value_eq_function(value, val_compare);
}

ioopm_hash_table_t *ioopm_hash_table_create(ioopm_eq_function key_eq, 
                                            ioopm_eq_function val_eq, 
                                            ioopm_hash_function hash_function)
{
    ioopm_hash_table_t *result = hash_table_create_custom(key_eq, val_eq, hash_function, Default_no_buckets, Default_load_factor);
    return result;
}

static ioopm_hash_table_t *hash_table_create_custom(ioopm_eq_function key_eq, 
                                                    ioopm_eq_function val_eq, 
                                                    ioopm_hash_function hash_function,
                                                    size_t buckets,
                                                    float load)
{
    /// Allocate space for a ioopm_hash_table_t = 17 pointers to
    /// entry_t's, which will be set to NULL
    ioopm_hash_table_t *result = calloc(1, sizeof(ioopm_hash_table_t));
    result->buckets = calloc(buckets, sizeof(entry_t));
    for(int i = 0; i < (int) buckets; ++i)
    {
        result->buckets[i].next = NULL;
    }
    result->key_eq_function = key_eq;
    result->value_eq_function = val_eq;
    result->hash_function = hash_function;
    result->no_buckets = buckets;
    result->load_factor = load;
    return result;
}

void ioopm_hash_table_destroy(ioopm_hash_table_t **ht)
{
    ioopm_hash_table_clear(*ht);
    free((*ht)->buckets);
    free(*ht);
    *ht = NULL;
}

static bool ht_load_level(ioopm_hash_table_t *ht)
{
    size_t size_of_ht = ioopm_hash_table_size(ht);
    float current_load = (float) size_of_ht / (float) ht->no_buckets;
    
    return current_load > (ht->load_factor);  //Om det aktuella "loaden" är större än vad som tillåts, returneras sant.
}

static void hash_table_grow(ioopm_hash_table_t *ht)
{
    //printf("Growing, no of bucket=%d\n", (int) ht->no_buckets);
    size_t primes[] = {17, 31, 67, 127, 257, 509, 1021, 2053, 4099, 8191, 16381, 32009, 65269};
    int primes_length = sizeof(primes) / sizeof(size_t); 
    size_t new_size;

    ioopm_list_t *keys = ioopm_hash_table_keys(ht);
    ioopm_list_t *values = ioopm_hash_table_values(ht);
    
    ioopm_list_iterator_t *key_iter = ioopm_list_iterator(keys);
    ioopm_list_iterator_t *value_iter = ioopm_list_iterator(values);
    
    for (int i = 0; i < primes_length - 1; ++i)
    {
        if (ht->no_buckets == primes[i])
        {
            new_size = primes[i+1];
        }
    }
    
    ioopm_hash_table_clear(ht);
    ht->buckets = realloc(ht->buckets, new_size*sizeof(entry_t));
    for(int i = 0; i < new_size; ++i)
    {
        ht->buckets[i].next = NULL;
    }
    ht->no_buckets = new_size;
    
    elem_t current_key, current_value;
    ioopm_iterator_current(key_iter, &current_key);
    ioopm_iterator_current(value_iter, &current_value);
    
    bool iter_has_value = ioopm_iterator_current(key_iter, &current_key) && ioopm_iterator_current(value_iter, &current_value);
    while(iter_has_value)
    {    
        ioopm_hash_table_insert(ht, current_key, current_value);
        iter_has_value = (ioopm_iterator_next(key_iter, &current_key) && ioopm_iterator_next(value_iter, &current_value));
    }
    
    ioopm_iterator_destroy(&key_iter);
    ioopm_iterator_destroy(&value_iter);
    ioopm_linked_list_destroy(keys);
    ioopm_linked_list_destroy(values);
}

static entry_t *find_previous_entry_for_key(ioopm_hash_table_t *ht, entry_t *entry, elem_t key)
{
    while (entry->next != NULL)
    {
        if (ht->hash_function(entry->next->key) >= ht->hash_function(key))
        {
            return entry;
        }
        else
        {
            entry = entry->next;
        }
    }
    return entry;
}

static entry_t *entry_create(ioopm_hash_table_t *ht, elem_t key, elem_t value, entry_t *next)
{
    entry_t *new_entry = calloc(1, sizeof(entry_t));
    new_entry->key = key;
    new_entry->value = value;
    new_entry->next = next;     
    return new_entry;
}

bool ioopm_hash_table_insert(ioopm_hash_table_t *ht, elem_t key, elem_t value)
{
    if (ht->hash_function(key) <= 0)
    {
        errno = EINVAL;
        return false;
    }
    
    if (ht_load_level(ht))
    {
        hash_table_grow(ht);
    }
    
    /// Calculate the bucket for this entry
    int bucket = ht->hash_function(key) % ht->no_buckets;
    /// Search for an existing entry for a key
    entry_t *tmp = find_previous_entry_for_key(ht, &ht->buckets[bucket], key);
    entry_t *current_entry = tmp->next;
    
    /// Check if the next entry should be updated or not
    if (current_entry != NULL && ht->key_eq_function(current_entry->key, key))
    {
        current_entry->value = value;
    }
    else
    {
        tmp->next = entry_create(ht, key, value, current_entry);
    }
    
    ht->size += 1;
    return true;
}



bool ioopm_hash_table_lookup(ioopm_hash_table_t *ht, elem_t key, elem_t *result)
{
    if (ht->hash_function(key) <= 0)
    {
        errno = EINVAL;
        return false;
    }
    /// Find the previous entry for key
    int bucket = ht->hash_function(key) % ht->no_buckets;
    entry_t *tmp = find_previous_entry_for_key(ht, &ht->buckets[bucket], key);
    entry_t *current_entry = tmp->next;
    
    if (current_entry && ht->key_eq_function(current_entry->key, key))
    {
        *result = current_entry->value;
        return true; 
    }
    else
    {
        return false; 
    }
}

bool ioopm_hash_table_remove_w_key(ioopm_hash_table_t *ht, elem_t key, elem_t *result, elem_t *key_res)
{
    if (ht->hash_function(key) <= 0) //Om key inte är vad som förväntas
    {
        errno = EINVAL;
        return false;
    }
    
    int bucket = ht->hash_function(key) % ht->no_buckets; 
    entry_t *tmp = find_previous_entry_for_key(ht, &ht->buckets[bucket], key);
    entry_t *current_entry = tmp->next;
    
    if (current_entry && ht->key_eq_function(current_entry->key, key))
    {
        //Om du tar bort element 1, [2], 3
        if(current_entry->next != NULL)
        {
            tmp->next = current_entry->next;
        }
        //Om du tar bort element 1, 2, [3]
        else
        {
            tmp->next = NULL;
        }
        if(key_res != NULL)
        {
            *key_res = current_entry->key;
        }
        *result = current_entry->value;
        entry_destroy(&current_entry);
        ht->size -= 1;
        return true;
    }
    
    return false;
}


bool ioopm_hash_table_remove(ioopm_hash_table_t *ht, elem_t key, elem_t *result)
{
    return ioopm_hash_table_remove_w_key(ht, key, result, NULL);
}

static void entry_destroy(entry_t **entry_to_destroy)
{
    free(*entry_to_destroy);
    *entry_to_destroy = NULL;
}

static void entries_destroy_all_iterativ(entry_t **e)
{
    entry_t *current_entry = *e;
    entry_t *next_entry;
    
    while (current_entry)
    {
        next_entry = current_entry->next;
        entry_destroy(&current_entry);
        current_entry = next_entry;
    }
    *e = NULL;
}

size_t ioopm_hash_table_size(ioopm_hash_table_t *ht)
{
    return ht->size;
}

bool ioopm_hash_table_is_empty(ioopm_hash_table_t *ht)
{
    return 0 == ioopm_hash_table_size(ht);
}

void ioopm_hash_table_clear(ioopm_hash_table_t *ht)
{
    for (int i = 0; i < ht->no_buckets; i++)
    {
        if (ht->buckets[i].next)
        {
            entries_destroy_all_iterativ(&(ht->buckets[i].next));
        }
    }   
    ht->size = 0;
}


ioopm_list_t *ioopm_hash_table_keys(ioopm_hash_table_t *ht)
{
    ioopm_list_t *list_of_keys = ioopm_linked_list_create(ht->key_eq_function);
    
    for(int i = 0; i < ht->no_buckets; ++i)
    {
        entry_t *current_entry = &(ht->buckets[i]);
        while(current_entry->next)
        {
            ioopm_linked_list_append(list_of_keys, current_entry->next->key);
            current_entry = current_entry->next;
        }  
    }
    
    return list_of_keys;
}

ioopm_list_t *ioopm_hash_table_values(ioopm_hash_table_t *ht)
{
    ioopm_list_t *list_of_values = ioopm_linked_list_create(ht->value_eq_function);
    
    for(int i = 0; i < ht->no_buckets; ++i)
    {
        entry_t *current_entry = &(ht->buckets[i]);
        while(current_entry->next)
        {
            ioopm_linked_list_append(list_of_values, current_entry->next->value);
            current_entry = current_entry->next;
        }  
    }
    return list_of_values;
}

bool ioopm_hash_table_has_key(ioopm_hash_table_t *ht, elem_t key)
{
    return ioopm_hash_table_any(ht, key_equiv, &key);    
}

bool ioopm_hash_table_has_value(ioopm_hash_table_t *ht, elem_t value)
{
    return ioopm_hash_table_any(ht, val_equiv, &value);
}


bool ioopm_hash_table_all(ioopm_hash_table_t *ht, ioopm_predicate pred, void *arg)
{
    size_t size = ioopm_hash_table_size(ht);
    ioopm_list_t *keys = ioopm_hash_table_keys(ht);
    ioopm_list_t *values = ioopm_hash_table_values(ht);
    bool result = true;

    for (int i = 0; i < size; ++i)
    {
        if(!pred(ht, ioopm_linked_list_get(keys, i), ioopm_linked_list_get(values, i), arg))
        {
            result = false;
            break;
        } 
    }
    ioopm_linked_list_destroy(values);
    ioopm_linked_list_destroy(keys);
    return result;
}

bool ioopm_hash_table_any(ioopm_hash_table_t *ht, ioopm_predicate pred, void *arg)
{
    ioopm_list_t *keys = ioopm_hash_table_keys(ht);
    ioopm_list_t *values = ioopm_hash_table_values(ht);
    bool result = false;
    
    ioopm_list_iterator_t *key_iter = ioopm_list_iterator(keys);
    ioopm_list_iterator_t *value_iter = ioopm_list_iterator(values);
    
    elem_t current_key, current_value;
    ioopm_iterator_current(key_iter, &current_key);
    ioopm_iterator_current(value_iter, &current_value);
    
    bool iter_has_value = ioopm_iterator_current(key_iter, &current_key) && ioopm_iterator_current(value_iter, &current_value);
    while(iter_has_value)
    {    
    
        if(pred(ht, current_key, current_value, arg))
        {
            result = true;
            break;
        }
         iter_has_value = (ioopm_iterator_next(key_iter, &current_key) && ioopm_iterator_next(value_iter, &current_value));
    }
    
    ioopm_linked_list_destroy(keys);
    ioopm_linked_list_destroy(values);
    ioopm_iterator_destroy(&key_iter);
    ioopm_iterator_destroy(&value_iter);
    return result;
}

void ioopm_hash_table_apply_to_all(ioopm_hash_table_t *ht, ioopm_apply_function apply_fun, void *arg)
{ 
    size_t size = ioopm_hash_table_size(ht);
    ioopm_list_t *keys = ioopm_hash_table_keys(ht);
    ioopm_list_t *values = ioopm_hash_table_values(ht);
    
    for (int i = 0; i < size; ++i)
    {
        apply_fun(ht, ioopm_linked_list_get(keys, i), ioopm_linked_list_get(values, i), arg); 
    }
    ioopm_linked_list_destroy(keys);
    ioopm_linked_list_destroy(values);
}