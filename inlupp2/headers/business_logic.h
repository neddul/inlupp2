#pragma once

#include "generic_data_structures.h"

typedef struct webstore_db db_t;

db_t *create_webstore();

bool bl_add_merchandise(db_t *db, char *merch_name, char *merch_desc, int price);

void bl_list_merchandise(db_t *db);

bool bl_remove_merchandise(db_t *db, char *merch_name);

void bl_edit_merchandise();

void bl_show_stock();

void bl_replenish();

void bl_create_cart();

void bl_remove_cart();

void bl_add_to_cart();

void bl_remove_from_cart();

void bl_calculate_cost();

void bl_checkout();

void bl_quit();