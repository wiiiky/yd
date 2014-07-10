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
#include "whashtable.h"
#include <stdio.h>
#include <stdlib.h>
#include <inttypes.h>
#include <string.h>

typedef struct {
    void *key;
    void *value;
} WHashTableNode;

static WHashTableNode *w_hash_table_node_new(void *key, void *value)
{
    WHashTableNode *node =
        (WHashTableNode *) malloc(sizeof(WHashTableNode));
    node->key = key;
    node->value = value;
    return node;
}

static void w_hash_table_node_free(WHashTableNode * node,
                                   WKeyDestroyFunc key_func,
                                   WValueDestroyFunc value_func)
{
    if (key_func) {
        key_func(node->key);
    }
    if (value_func) {
        value_func(node->value);
    }
    free(node);
}

/*
 * WHashTable
 */
struct _WHashTable {
    uint32_t size;              /* size of bucket */
    uint32_t mod;               /* a prime number <= size */

    WHashFunc hash_func;
    WEqualFunc equal_func;
    WKeyDestroyFunc key_func;
    WValueDestroyFunc value_func;

    WList *keys;

    WList **buckets;            /* buckets */
};

/* Each table size has an associated prime modulo (the first prime
 * lower than the table size) used to find the initial bucket. Probing
 * then works modulo 2^n. The prime modulo is necessary to get a
 * good distribution with poor hash functions.
 */
static const uint32_t prime_mod[] = {
    1,                          /* For 1 << 0 */
    2,
    3,
    7,
    13,
    31,
    61,
    127,
    251,
    509,
    1021,
    2039,
    4093,
    8191,
    16381,
    32749,
    65521,                      /* For 1 << 16 */
    131071,
    262139,
    524287,
    1048573,
    2097143,
    4194301,
    8388593,
    16777213,
    33554393,
    67108859,
    134217689,
    268435399,
    536870909,
    1073741789,
    2147483647                  /* For 1 << 31 */
};

/*
 * 31 buckets by default
 */
#define W_HASH_TABLE_DEFAULT_INDEX  (5)

WHashTable *w_hash_table_new(unsigned short i,
                             WHashFunc hash_func,
                             WEqualFunc equal_func,
                             WKeyDestroyFunc key_func,
                             WValueDestroyFunc value_func)
{
    unsigned short index;
    /* I found that malloc(sizeof(char*)*(1<<31)) will fail in my system
       but malloc(sizeof(char*)*(1<<30)) not */
    if (i >= 31) {
        index = 31;
    } else if (i < 0) {
        index = W_HASH_TABLE_DEFAULT_INDEX;
    } else {
        index = i;
    }

    WHashTable *h = (WHashTable *) malloc(sizeof(WHashTable));
    h->size = prime_mod[index];
    h->mod = prime_mod[index];
    h->buckets = (WList **) calloc(sizeof(WList *), h->size);   /* init to NULL */
    h->keys = NULL;
    h->equal_func = equal_func;
    h->hash_func = hash_func;
    h->key_func = key_func;
    h->value_func = value_func;

    return h;
}

/*
 * return the bucket index of the key
 */
static inline uint32_t w_hash_table_index(WHashTable * h, void *key)
{
    uint32_t hash_code = h->hash_func(key);
    uint32_t index = hash_code % h->mod;
    return index;
}

/*
 * return the node that contains given key
 */
static inline WHashTableNode *w_hash_table_find_node(WHashTable * h,
                                                     const void *key)
{
    WList *lp = h->keys;
    while (lp) {
        if (h->equal_func(lp->data, key) == 0) {
            break;
        }
        lp = w_list_next(lp);
    }
    if (lp == NULL) {
        return NULL;
    }

    uint32_t index = w_hash_table_index(h, lp->data);
    WList *bucket = h->buckets[index];
    WHashTableNode *node = NULL;
    while (bucket) {
        WHashTableNode *data = (WHashTableNode *) bucket->data;
        if (data->key == lp->data) {
            node = data;
            break;
        }
        bucket = w_list_next(bucket);
    }
    return node;
}

int w_hash_table_insert(WHashTable * h, void *key, void *value)
{
    WHashTableNode *node = w_hash_table_find_node(h, key);
    if (node == NULL) {         /* if not exists, insert */
        uint32_t index = w_hash_table_index(h, key);
        node = w_hash_table_node_new(key, value);
        h->buckets[index] = w_list_append(h->buckets[index], node);
        h->keys = w_list_append(h->keys, key);
        return 0;
    }
    /* if already exists, update */
    if (h->value_func) {        /* free old value */
        h->value_func(node->value);
    }
    node->value = value;
    return 1;
}

int w_hash_table_update(WHashTable * h, void *key, void *value)
{
    WHashTableNode *node = w_hash_table_find_node(h, key);
    if (node == NULL) {         /* not found */
        return -1;
    }
    if (h->value_func) {        /* free old value */
        h->value_func(node->value);
    }
    node->value = value;
    return 0;
}

static inline int w_hash_table_remove_internal(WHashTable * h, void *key,
                                               WKeyDestroyFunc key_func,
                                               WValueDestroyFunc
                                               value_func)
{
    WHashTableNode *node = w_hash_table_find_node(h, key);

    if (node) {
        uint32_t index = w_hash_table_index(h, key);
        h->keys = w_list_remove(h->keys, key);
        h->buckets[index] = w_list_remove(h->buckets[index], node);
        w_hash_table_node_free(node, key_func, value_func);
        return 0;
    }
    return -1;
}

/* remove but not free key or value */
int w_hash_table_remove(WHashTable * h, void *key)
{
    return w_hash_table_remove_internal(h, key, NULL, NULL);
}

/* remove and free key and value */
int w_hash_table_remove_full(WHashTable * h, void *key)
{
    return w_hash_table_remove_internal(h, key, h->key_func,
                                        h->value_func);
}

void *w_hash_table_find(WHashTable * h, const void *key)
{
    WHashTableNode *node = w_hash_table_find_node(h, key);

    if (node) {
        return node->value;
    }

    return NULL;
}

void w_hash_table_foreach(WHashTable * h, WNodeFunc node_func, void *data)
{
    WList *key = h->keys;
    while (key) {
        WHashTableNode *node = w_hash_table_find_node(h, key->data);
        if (node) {
            node_func(node->key, node->value, data);
        }
        key = w_list_next(key);
    }
}

WList *w_hash_table_get_keys(WHashTable * h)
{
    return h->keys;
}


static void w_hash_table_free_internal(WHashTable * h,
                                       WKeyDestroyFunc key_func,
                                       WValueDestroyFunc value_func)
{
    uint32_t i;
    for (i = 0; i < h->size; i++) {
        WList *list = h->buckets[i];
        if (list) {
            WList *lp = w_list_next(list);
            while (list) {
                lp = w_list_next(list);
                w_hash_table_node_free(list->data, key_func, value_func);
                free(list);
                list = lp;
            }
        }
    }
    free(h->buckets);
    w_list_free(h->keys);
    free(h);
}

void w_hash_table_free_full(WHashTable * h)
{
    w_hash_table_free_internal(h, h->key_func, h->value_func);
}

void w_hash_table_free(WHashTable * h)
{
    w_hash_table_free_internal(h, NULL, NULL);
}


unsigned int w_str_hash(const void *p)
{
    const char *s = (const char *) p;
    uint32_t hash = 0;
    while (*s) {
        hash = (hash << 4) + *s;
        s++;
    }
    return hash;
}

int w_str_equal(const void *s1, const void *s2)
{
    return strcmp((const char *) s1, (const char *) s2);
}

unsigned int w_int_hash(const void *p)
{
    int i = (int) (long) p;
    return (unsigned int) i;
}

int w_int_equal(const void *p1, const void *p2)
{
    return (int) (p1 - p2);
}
