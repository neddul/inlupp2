#include "linked_list.h"

/// @brief Checks if there are more elements to iterate over
/// @param iter the iterator
/// @return true if next element is present
bool ioopm_iterator_has_next(ioopm_list_iterator_t *iter);

/// @brief Step the iterator forward one ste
/// @param iter the iterator
/// @param result pointer to an elem_t for storing the element 
/// @return true if we it could step next, else false
bool ioopm_iterator_next(ioopm_list_iterator_t *iter, elem_t *result);

/// @brief Reposition the iterator at the start of the underlying list
/// @param iter double ref pointer to iterator
void ioopm_iterator_reset(ioopm_list_iterator_t **iter);

/// @brief Fetch the current element from the underlying list
/// @param iter the iterator
/// @param result pointer to an elem_t for storing the element 
/// @return true if fetch was successful, else false 
bool ioopm_iterator_current(ioopm_list_iterator_t *iter, elem_t *result);

/// @brief Destroy the iterator, set the pointer to NULL and return its resources
/// @param iter double ref pointer to the iterator
void ioopm_iterator_destroy(ioopm_list_iterator_t **iter);