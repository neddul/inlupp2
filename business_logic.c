#include "headers/business_logic.h"
#include "headers/generic_utils.h"

#include <limits.h>

struct webstore_db
{
    ioopm_hash_table_t  *merch;
    ioopm_hash_table_t  *storage;
    ioopm_hash_table_t  *carts;
    int                 carts_created;
};

typedef struct webstore_db db_t;

struct merch
{
    char            *name;
    char            *desc;
    int             price;
    int             stock;
    ioopm_list_t    *locs;
};

typedef struct merch merch_t;

struct shelf
{
    char    *shelf_name;
    int     quantity;
};

typedef struct shelf shelf_t;

static bool string_key_eq(elem_t k1, elem_t k2)
{
  return (strcmp(k1.str_val, k2.str_val) == 0);    ; // if elem_t has field char *string
}

static bool int_key_eq(elem_t k1, elem_t k2)
{
    return k1.int_val == k2.int_val;
}


static bool shelf_comp(elem_t a, elem_t b)
{
    shelf_t *shelf_a = a.ptr_val;
    shelf_t *shelf_b = b.ptr_val;
    return(strcmp(shelf_a->shelf_name, shelf_b->shelf_name) == 0);
}

/*=================================================================
 *  Hash Functions
 *=================================================================*/

int string_knr_hash(elem_t key)
{
    char *str = key.str_val;
    unsigned long result = 0;
    do
    {
        result = result * 31 + *str;
    }
    while (*++str != '\0');
    return result % INT_MAX;
}

int int_knr_hash(elem_t key)
{
    return key.int_val;
}

db_t *create_webstore()
{
    db_t *webstore = calloc(1, sizeof(db_t));
    webstore->merch         = ioopm_hash_table_create(string_key_eq, string_key_eq, string_knr_hash);
    webstore->storage       = ioopm_hash_table_create(string_key_eq, string_key_eq, string_knr_hash);
    webstore->carts         = ioopm_hash_table_create(int_key_eq, false, int_knr_hash);
    webstore->carts_created = 0;
    
    return webstore;
}

void destroy_webstore(db_t *webstore)
{
    ioopm_hash_table_destroy(&webstore->merch);
    ioopm_hash_table_destroy(&webstore->storage);
    ioopm_hash_table_destroy(&webstore->carts);
    free(webstore);
    
}
//------------------------------------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------------------------------------
//------------------------------ Start of add merchandise

merch_t *create_merch(char *merch_name, char *merch_desc, int merch_price)
{
    merch_t *new_merch = calloc(1, sizeof(merch_t));
    new_merch->name     = merch_name;
    new_merch->desc     = merch_desc;
    new_merch->price    = merch_price;
    new_merch->stock    = 0;            //When new merch is added, stock is always 0.
    new_merch->locs     = ioopm_linked_list_create(shelf_comp);
    return new_merch;
}

bool merch_exists(db_t *db, char *merch_name)
{
    elem_t ignore_value;
    return ioopm_hash_table_lookup(db->merch, str_elem(merch_name), &ignore_value);
}

bool bl_add_merchandise(db_t *db, char *merch_name, char *merch_desc, int price)
{
    if(merch_exists(db, merch_name) || price < 1)
    {
        return false; //The merch already exists or an invalid price has been set, nothing will be done
    }
    
    merch_t *new_merch = create_merch(merch_name, merch_desc, price);
    
    return ioopm_hash_table_insert(db->merch, str_elem(merch_name), ptr_elem(new_merch));
}

//------------------------------ End of add merchandise
//------------------------------------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------------------------------------



//------------------------------------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------------------------------------
//------------------------------ Start of remove merchandise

void destroy_shelf(db_t *db, char *shelf_name)
{
    elem_t result;
    elem_t gotten_shelf;
    ioopm_hash_table_lookup(db->storage, str_elem(shelf_name), &result);
    
    merch_t *merch;
    merch = get_merch(db, result.str_val);
    
    ioopm_list_t *list = merch->locs;
    int no_of_storage_locs = ioopm_linked_list_size(list);
    int i = 0;
    shelf_t *shelf;
    for(; i < no_of_storage_locs; i++)
    {
        
        gotten_shelf = ioopm_linked_list_get(list, i);
        shelf = (shelf_t *) gotten_shelf.ptr_val;
        
        if(strcmp(shelf->shelf_name, shelf_name)==0)
        {
            ioopm_linked_list_remove(list, i, &gotten_shelf); 
            free(shelf->shelf_name);
            free(shelf);
            i = no_of_storage_locs; //We break the loop
        }
        
        
    }
    
    free(result.str_val);
    ioopm_hash_table_remove(db->storage, str_elem(shelf_name), &result);
}

merch_t *get_merch(db_t *db, char *merch_name)
{
    merch_t *merch;
    elem_t hashed_merch;

    ioopm_hash_table_lookup(db->merch, str_elem(merch_name), &hashed_merch);
    
    merch = (merch_t *) hashed_merch.ptr_val;
    return merch;
}

void destroy_stock(db_t *db, char *merch_name)
{
    merch_t *merch;
    merch = get_merch(db, merch_name);
    
    elem_t res;
    ioopm_list_iterator_t *iter = ioopm_list_iterator(merch->locs);
    
    bool has_next = true;
    shelf_t *shelf;
    
    while(has_next)
    {
        
        ioopm_iterator_current(iter, &res);
        shelf = (shelf_t *) res.ptr_val;
        destroy_shelf(db, shelf->shelf_name);
        has_next = ioopm_iterator_next(iter, &res);
        
    }
    
    ioopm_iterator_destroy(&iter);
    
}

bool destroy_merch(db_t *db, char *merch_name)
{
    merch_t *merch;
    merch = get_merch(db, merch_name);
    
    if(merch->stock > 0) //The merch has no stock
    {
        destroy_stock(db, merch_name);
    }
    
    ioopm_linked_list_destroy(merch->locs);
    
    bool remove_result;        
    remove_result = ioopm_hash_table_remove(db->merch, str_elem(merch_name), &ignore_value);
    free(merch->name);
    free(merch->desc);
    free(merch);
    
    
    return remove_result;
}

bool bl_remove_merchandise(db_t *db, char *merch_name)
{
    if(!merch_exists(db, merch_name))
    {
        return false; //The merch does not exist. Nothing is removed
    }
    
    destroy_merch(db, merch_name);
    
    return true;
}

//------------------------------ End of remove merchandise
//------------------------------------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------------------------------------



//------------------------------------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------------------------------------
//------------------------------ Start of list merchandise

char **get_all_keys(ioopm_hash_table_t *ht)
{
    
    size_t no_keys = ioopm_hash_table_size(ht);
    char **keys = calloc(no_keys, sizeof(char*));
    if(no_keys > 0)
    {
        
        ioopm_list_t *list = ioopm_hash_table_keys(ht);
        
        ioopm_list_iterator_t *iter = ioopm_list_iterator(list);
        elem_t = res;
        
        bool iter_has_value = ioopm_iterator_current(iter, &res);
        
        for(int i = 0; iter_has_value; i++)
        {
            keys[i] = res.str_val;
            iter_has_value = ioopm_iterator_next(iter, &res);
        }
        
        ioopm_iterator_destroy(&iter);
        return keys;
    }
    
    //There are no merch 
    free(keys);
    keys = calloc(1, sizeof(char*));
    keys[0] = NULL;
    return keys;
}


void bl_list_merchandise(db_t *db)
{
    size_t no_merch = ioopm_hash_table_size(db->merch);
    
    if(no_merch > 0)
    {
        char **all_merch;
        
        all_merch = get_all_keys(db->merch);
        no_merch = ioopm_hash_table_size(db->merch);
        
        sort_keys(all_merch, no_merch);
        
        for(int i = 0; i < no_merch; i++)
        {
            printf("%d. %s\n", i+1, all_merch[i]);
        }
        free(all_merch)
    }
    
    
    
}




int main()
{
    db_t *yo = create_webstore();
    
    char *name = ask_question_string("name\n");
    char *desc = ask_question_string("desc\n");
    
    bl_add_merchandise(yo, name, desc, 4);
    printf("Hello world!\n");
    bl_remove_merchandise(yo, name);
    destroy_webstore(yo);
    return 0;
}