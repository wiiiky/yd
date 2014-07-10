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

#include "wlist.h"
#include <stdlib.h>

WList *w_list_new()
{
    return NULL;
}

static inline void w_list_element_free(WList * list, WListDestroy destroy)
{
    if (destroy) {
        destroy(w_list_data(list));
    }
    free(list);
}

WList *w_list_first(WList * list)
{
    while (w_list_prev(list)) {
        list = w_list_prev(list);
    }
    return list;
}

WList *w_list_last(WList * list)
{
    while (w_list_next(list)) {
        list = w_list_next(list);
    }
    return list;
}

static inline WList *w_list_append_element(WList * list, WList * ele)
{
    if (list == NULL) {
        return ele;
    }
    WList *last = w_list_last(list);
    last->next = ele;
    ele->prev = last;
    ele->next = NULL;
    return list;
}

WList *w_list_append(WList * list, void *data)
{
    WList *ele = w_list_alloc(data);

    return w_list_append_element(list, ele);
}

WList *w_list_insert(WList * list, void *data, int position)
{
    if (position < 0 || list == NULL) {
        /* if position <0, append data to the end of the list */
        /* if list == NULL, whatever position is */
        return w_list_append(list, data);
    }
    int i = 0;
    WList *ptr = list;
    while (i < position && ptr) {
        i++;
        ptr = w_list_next(ptr);
    }
    if (ptr == NULL) {
        return w_list_append(list, data);
    }

    WList *ele = w_list_alloc(data);

    /* insert data before ptr */
    if (w_list_prev(ptr) == NULL) {
        /* insert data to the first of list */
        ele->next = ptr;
        ptr->prev = ele;
        return ele;
    }
    ptr->prev->next = ele;
    ele->next = ptr;
    ele->prev = ptr->prev;
    ptr->prev = ele;
    return list;
}

void w_list_foreach(WList * list, WForeachFunc func, void *user_data)
{
    while (list) {
        /* call function for each element */
        func(list->data, user_data);
        list = w_list_next(list);
    }
}

static void w_list_free_internal(WList * list, WListDestroy destroy)
{
    if (list == NULL) {
        return;
    }
    WList *lp = w_list_next(list);
    while (list) {
        lp = w_list_next(list);
        if (destroy) {
            destroy(list->data);
        }
        free(list);
        list = lp;
    }
}

void w_list_free(WList * list)
{
    w_list_free_internal(list, NULL);
}

void w_list_free_full(WList * list, WListDestroy destroy)
{
    w_list_free_internal(list, destroy);
}

WList *w_list_find_custom(WList * list, WCompareFunc func, const void *b)
{
    while (list) {
        if (func(list->data, b) == 0) {
            return list;
        }
        list = w_list_next(list);
    }
    return NULL;
}

/*
 * 将链表中一个元素提取出来，返回新链表首部
 * 
 * 比如一个链表：A B C D E F G
 * 
 * 那么element可以是其中任何一个元素，
 * 如果element是B，那么新链表是A C D E F G，返回A的指针，B被设置为一个独立的元素
 * 如果element是A，那么新链表是B C D E F G，返回B的指针，A被设置为一个独立的元素
 *
 */
static WList *w_list_element_detach(WList * element)
{
    WList *prev = w_list_prev(element);
    WList *next = w_list_next(element);
    WList *list = NULL;
    if (prev) {
        list = w_list_first(prev);
        prev->next = next;
    } else {
        list = next;
    }
    if (next) {
        next->prev = prev;
    }
    element->next = NULL;
    element->prev = NULL;
    return list;
}

WList *w_list_sort_bubble(WList * list, WCompareFunc func)
{
    WList *new = NULL, *head = list;
    while (head) {
        WList *ptr = w_list_next(head);
        WList *ele = head;
        while (ptr) {
            if (func(w_list_data(ele), w_list_data(ptr)) > 0) {
                ele = ptr;
            }
            ptr = w_list_next(ptr);
        }
        head = w_list_element_detach(ele);
        new = w_list_append_element(new, ele);
    }
    return new;
}

/*
 * insert an element into the list and
 * use given compare function to determine its position
 */
static WList *w_list_insert_element_sorted(WList * list, WList * ele,
                                           WCompareFunc func)
{
    ele->next = NULL;
    ele->prev = NULL;
    if (list == NULL) {
        list = ele;
    } else if (func(w_list_data(ele), w_list_data(list)) < 0) {
        /* insert element into the first position */
        ele->next = list;
        list->prev = ele;
        list = ele;
    } else {
        WList *nptr = w_list_next(list);
        while (nptr) {
            if (func(w_list_data(ele), w_list_data(nptr)) < 0) {
                WList *prev = w_list_prev(nptr);
                ele->next = nptr;
                nptr->prev = ele;
                ele->prev = prev;
                prev->next = ele;
                break;
            }
            if (w_list_next(nptr)) {
                nptr = w_list_next(nptr);
            } else {
                /* append to last */
                nptr->next = ele;
                ele->prev = nptr;
                break;
            }
        }
    }
    return list;
}

WList *w_list_sort_insertion(WList * list, WCompareFunc func)
{
    WList *new = NULL;
    WList *ptr = list;
    while (ptr) {
        WList *ele = ptr;
        ptr = w_list_next(ptr);
        new = w_list_insert_element_sorted(new, ele, func);
    }
    return new;
}

WList *w_list_insert_sorted(WList * list, void *data, WCompareFunc func)
{
    WList *ele = w_list_alloc(data);
    return w_list_insert_element_sorted(list, ele, func);
}

WList *w_list_find(WList * list, void *data)
{
    while (list) {
        if (list->data == data) {
            return list;
        }
        list = w_list_next(list);
    }
    return NULL;
}

/*
 * 从链表中删除指定的元素，如果destroy不为空，对元素的数据调用destroy函数
 */
static WList *w_list_remove_internal(WList * list, void *data,
                                     WListDestroy destroy)
{
    WList *rm = w_list_find(list, data);
    if (rm == NULL) {           /* not find */
        return list;
    }
    list = w_list_element_detach(rm);
    w_list_element_free(rm, destroy);
    return list;
}

WList *w_list_remove(WList * list, void *data)
{
    return w_list_remove_internal(list, data, NULL);
}

WList *w_list_remove_full(WList * list, void *data, WListDestroy destroy)
{
    return w_list_remove_internal(list, data, destroy);
}

WList *w_list_reverse(WList * list)
{
    while (list) {
        WList *t = w_list_next(list);
        list->next = w_list_prev(list);
        list->prev = t;
        if (t) {
            list = t;
        } else {
            return list;
        }
    }
    /* never come here */
    return NULL;
}

WList *w_list_alloc(void *data)
{
    WList *list = (WList *) malloc(sizeof(WList));
    list->prev = list->next = NULL;
    list->data = data;
    return list;
}

void w_list_free1(WList * list)
{
    w_list_free1_full(list, NULL);
}

void w_list_free1_full(WList * list, WListDestroy destroy)
{
    if (w_list_data(list) && destroy) {
        destroy(w_list_data(list));
    }
    free(list);
}
