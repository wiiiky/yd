/*
 * Copyright (C) 2014  Wiky L
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 * 
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 * 
 * You should have received a copy of the GNU Lesser General Public
 * License along with main.c; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor Boston, MA 02110-1301,  USA
 */
#ifndef __W_WHASHTABLE_H__
#define __W_WHASHTABLE_H__

#include "wlist.h"
/*
 * WHashTable, a general hash table.
 * Remember that WHashTable never copys key or value, which means they should 
 * be accessed for the lifetime of WHashTable
 */

/*
 * The structure of WHastTable is not public
 */
typedef struct _WHashTable WHashTable;


/*
 * The hash function
 * 
 * @return: the hash code
 */
typedef unsigned int (*WHashFunc) (const void *data);
/*
 * The equal function
 * 
 * @return: zero if equal, non-zero if not.
 */
typedef int (*WEqualFunc) (const void *a, const void *b);
/*
 * the key & value destroy function
 */
typedef void (*WKeyDestroyFunc) (void *key);
typedef void (*WValueDestroyFunc) (void *value);

/*
 *
 */
typedef int (*WNodeFunc) (void *key, void *value, void *user_data);

/*
 * create a new WHastTable
 */
WHashTable *w_hash_table_new(unsigned short i,
                             WHashFunc hash_func,
                             WEqualFunc equal_func,
                             WKeyDestroyFunc key_func,
                             WValueDestroyFunc value_func);

/*
 * @description: Insert a new key:value into hash table
 *				 if the key already exists, just update its value
 * @param h: the hash table
 * @param key: the key
 * @param value: the value
 * 
 * @return: 0 if inserted, 1 if updated, -1 if fail
 */
int w_hash_table_insert(WHashTable * h, void *key, void *value);

/*
 * @description: Update the value associated to the given key,
 *				if not exists, do nothing
 * 
 * @param h: the hash table
 * @param key: the key
 * @param value: the new value of key.
 * 
 * @return: 0 if updated, -1 if key not found
 */
int w_hash_table_update(WHashTable * h, void *key, void *value);

/*
 * @description: Remove a node whoes key matches.
 *				 this function does not free key and value.
 * 
 * @param key: the key to remove.
 * 
 * @return: 0 if removed, -1 if key not found.
 */
int w_hash_table_remove(WHashTable * h, void *key);

/* remove and free */
int w_hash_table_remove_full(WHashTable * h, void *key);


/*
 * @description: find the value associated to to key
 * 
 * @return: the value if found, or NULL
 */
void *w_hash_table_find(WHashTable * h, const void *key);


/*
 * @description: call node_func on every element in the hash table.
 */
void w_hash_table_foreach(WHashTable * h, WNodeFunc node_func, void *data);

/*
 * @description: get all the keys in the hash table
 * 
 * @return: the list of keys, this list is owned by hash table,
 *			should not be modified.
 */
WList *w_hash_table_get_keys(WHashTable * h);


/*
 * @description: free a hash table
 *				 this function will call WKeyDestroyFunc and 
 *				 WValueDestroyFunc on every element.
 */
void w_hash_table_free_full(WHashTable * h);

/*
 * @descrition: just free hash table itself
 */
void w_hash_table_free(WHashTable * h);


/*
 * hash and equal functions
 */

/* string */
unsigned int w_str_hash(const void *p);
int w_str_equal(const void *s1, const void *s2);

/* int */
unsigned int w_int_hash(const void *p);
int w_int_equal(const void *p1, const void *p2);


#endif
