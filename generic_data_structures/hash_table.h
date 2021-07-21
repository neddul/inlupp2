#pragma once
#include "common.h"
#include "linked_list.h"
#include "iterator.h"


/**
 * @file hash_table.h
 * @author Jakob Nordgren & Ludvig Westerholm 
 * @date 1 Sep 2019
 * @brief Simple hash table that maps integer keys to string values.
 *
 * Here typically goes a more extensive explanation of what the header
 * defines. Doxygens tags are words preceeded by either a backslash @\
 * or by an at symbol @@.
 *
 * @see http://wrigstad.com/ioopm19/assignments/assignment1.html
 */
 
typedef struct hash_table ioopm_hash_table_t;
typedef bool(*ioopm_predicate)(ioopm_hash_table_t *ht, elem_t key, elem_t value, void *extra);
typedef void(*ioopm_apply_function)(ioopm_hash_table_t *ht, elem_t key, elem_t value, void *extra);
typedef int(*ioopm_hash_function)(elem_t key);

/// @brief Create a new hash table. The provovided hash function must return
/// only positive integers. 
/// @param key_eq pointer to function for comparing keys
/// @param val_eq pointer to function for comparing values
/// @param hash_function pointer to hashing function, which returns a positive integer 
/// @return A new empty hash table
ioopm_hash_table_t *ioopm_hash_table_create(ioopm_eq_function key_eq, 
                                            ioopm_eq_function val_eq, 
                                            ioopm_hash_function hash_function);

/// @brief Delete a hash table, free its memory and set its pointer to NULL
/// @param ht double ref pointer to a hash table to be deleted
void ioopm_hash_table_destroy(ioopm_hash_table_t **ht);

/// @brief add key => value entry in hash table ht
/// if key is not valid, errno is set to EINVAL
/// @param ht hash table operated upon
/// @param key key to insert
/// @param value value to insert
/// @return true is insert was successful, else false
bool ioopm_hash_table_insert(ioopm_hash_table_t *ht, elem_t key, elem_t value);

/// @brief lookup value for key in hash table ht
/// @param ht hash table operated upon
/// @param key key to lookup
/// @param result pointer to a string buffer for storing the lookup value 
/// @returns a truth statement whether a key exists or not. Returns false and sets errno if called with an invalid key 
bool ioopm_hash_table_lookup(ioopm_hash_table_t *ht, elem_t key, elem_t *result);

/// @brief remove any mapping from key to a value
/// if key is not valid, errno is set to EINVAL
/// @param ht hash table operated upon
/// @param key key to remove
/// @return true is removal was successful, else false
bool ioopm_hash_table_remove(ioopm_hash_table_t *ht, elem_t key, elem_t *result);

/// @brief remove any mapping from key to a value
/// if key is not valid, errno is set to EINVAL
/// @param ht hash table operated upon
/// @param key key to remove
/// @param result is removed value
/// @param key_res is removed key
/// @return true is removal was successful, else false
bool ioopm_hash_table_remove_w_key(ioopm_hash_table_t *ht, elem_t key, elem_t *result, elem_t *key_res);

/// @brief returns the number of key => value entries in the hash table
/// @param h hash table operated upon
/// @return the number of key => value entries in the hash table
size_t ioopm_hash_table_size(ioopm_hash_table_t *ht);

/// @brief checks if the hash table is empty
/// @param h hash table operated upon
/// @return true if size == 0, else false
bool ioopm_hash_table_is_empty(ioopm_hash_table_t *ht);

/// @brief clear all the entries in a hash table
/// @param h hash table operated upon
void ioopm_hash_table_clear(ioopm_hash_table_t *ht);

/// @brief return the keys for all entries in a hash map (in no particular order, but same as ioopm_hash_table_values)
/// @param h hash table operated upon
/// @return an ioopm_list_t, (a linked list), with the keys for a hash table h
ioopm_list_t *ioopm_hash_table_keys(ioopm_hash_table_t *ht);

/// @brief return the values for all entries in a hash map (in no particular order, but same as ioopm_hash_table_keys)
/// @param h hash table operated upon
/// @return an ioopm_list_t, (a linked list), with the values for a hash table h
ioopm_list_t *ioopm_hash_table_values(ioopm_hash_table_t *ht);

/// @brief check if a hash table has an entry with a given key
/// @param h hash table operated upon
/// @param key the key sought
/// @return true if key is present, else false
bool ioopm_hash_table_has_key(ioopm_hash_table_t *ht, elem_t key);

/// @brief check if a hash table has an entry with a given value
/// @param h hash table operated upon
/// @param value the value sought
/// @return true if value is present, else false
bool ioopm_hash_table_has_value(ioopm_hash_table_t *ht, elem_t value);

/// @brief check if a predicate is satisfied by all entries in a hash table
/// @param h hash table operated upon
/// @param pred the predicate
/// @param arg extra argument to pred
/// @return true if predicate is true, else false
bool ioopm_hash_table_all(ioopm_hash_table_t *ht, ioopm_predicate pred, void *arg);

/// @brief check if a predicate is satisfied by any entry in a hash table
/// @param h hash table operated upon
/// @param pred the predicate
/// @param arg extra argument to pred
/// @return true if the predicate is satisfied, else false
bool ioopm_hash_table_any(ioopm_hash_table_t *ht, ioopm_predicate pred, void *arg);

/// @brief apply a function to all entries in a hash table
/// @param h hash table operated upon
/// @param apply_fun the function to be applied to all elements
/// @param arg extra argument to apply_fun
void ioopm_hash_table_apply_to_all(ioopm_hash_table_t *ht, ioopm_apply_function apply_fun, void *arg);



