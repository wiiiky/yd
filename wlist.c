/*=============================================================================
#     FileName: wlist.c
#         Desc: double linked list
#       Author: Wiky L
#        Email: wiiiky@yeah.net
#     HomePage: https://launchpad.net/~wiiiky-v
#      Version: 0.0.1
#   LastChange: 2013-09-21 09:27:27
#      History:
=============================================================================*/

#include "wlist.h"
#include <stdlib.h>
#include <string.h>

/* make l2 before l1 */
static void _w_list_before(WList * l1, WList * l2)
{
    l2->next = l1;
    l2->prev = l1->prev;
    if (l1->prev != NULL)
        l1->prev->next = l2;
    l1->prev = l2;
}

/* make l2 after l1 */
static void _w_list_after(WList * l1, WList * l2)
{
    l2->prev = l1;
    l2->next = l1->next;
    if (l1->next)
        l1->next->prev = l2;
    l1->next = l2;
}

WList *w_list_append(WList * list, void *data)
{
    if (list == NULL)
        return w_list_alloc(data);

    WList *new = w_list_alloc(data);
    WList *last = w_list_last(list);
    last->next = new;
    new->prev = last;
    return w_list_first(list);
}

WList *w_list_prepend(WList * list, void *data)
{
    if (list == NULL)
        return w_list_alloc(data);

    WList *new = w_list_alloc(data);
    WList *first = w_list_first(list);
    first->prev = new;
    new->next = first;
    return new;
}

WList *w_list_insert(WList * list, void *data, unsigned int position)
{
    if (position == 0)
        return w_list_prepend(list, data);
    else if (position > w_list_length(list))    /* if postion==w_list_length(list), the new element should be inserted before the last element */
        return w_list_append(list, data);

    WList *ptr = w_list_first(list);
    unsigned int n = 1;
    while (n != position) {
        ptr = w_list_next(ptr);
        n++;
    }
    /* insert after ptr */
    WList *new = w_list_alloc(data);
    _w_list_after(ptr, new);

    return w_list_first(list);
}

WList *w_list_insert_sorted(WList * list, WCompareFunc func, void *data)
{
    if (list == NULL)
        return w_list_alloc(data);

    WList *ptr = w_list_first(list);
    while (ptr != NULL) {
        if (func(ptr->data, data) > 0)
            break;
        ptr = w_list_next(ptr);
    }
    /* insert the new element before ptr */
    if (ptr == NULL) {
        return w_list_append(list, data);
    }
    WList *new = w_list_alloc(data);
    _w_list_before(ptr, new);
    return w_list_first(list);

}

WList *w_list_remove(WList * list, const void *data)
{
    WList *ptr;

    ptr = w_list_first(list);
    while (ptr != NULL) {
        if (ptr->data == data)
            break;
        ptr = w_list_next(ptr);
    }
    if (ptr == NULL)
        return list;
    /* here to remove ptr */
    list = NULL;
    if (ptr->next != NULL) {
        ptr->next->prev = ptr->prev;
        list = w_list_first(ptr->next);
    }
    if (ptr->prev != NULL) {
        ptr->prev->next = ptr->next;
        list = w_list_first(ptr->prev);
    }

    free(ptr);
    return list;
}

WList *w_list_remove_all(WList * list, const void *data)
{
    WList *ptr, *next;
    ptr = w_list_first(list);
    while (ptr != NULL) {
        if (ptr->data == data) {
            list = w_list_remove(list, data);
            ptr = list;
            continue;
        }
        ptr = w_list_next(ptr);
    }
    return w_list_first(list);
}

void w_list_free(WList * list)
{
    WList *ptr, *next;
    ptr = w_list_first(list);
    while (ptr != NULL) {
        next = w_list_next(ptr);
        free(ptr);
        ptr = next;
    }
}

void w_list_free_full(WList * list, WDestroyNotify func)
{
    WList *ptr, *next;
    ptr = w_list_first(list);
    while (ptr != NULL) {
        next = w_list_next(ptr);
        func(ptr->data);
        free(ptr);
        ptr = next;
    }
}

unsigned int w_list_length(WList * list)
{
    WList *ptr;
    unsigned int n;

    ptr = w_list_first(list);
    n = 0;
    while (ptr != NULL) {
        n++;
        ptr = w_list_next(ptr);
    }
    return n;
}

WList *w_list_copy(WList * list)
{
    WList *new, *ptr;
    ptr = w_list_first(list);
    new = NULL;
    while (ptr != NULL) {
        new = w_list_append(new, ptr->data);
        ptr = w_list_next(ptr);
    }

    return new;
}

WList *w_list_copy_deep(WList * list, WCopyFunc func, void *data)
{
    WList *new, *ptr;
    void *new_data;
    ptr = w_list_first(list);
    new = NULL;
    while (ptr != NULL) {
        new_data = func(ptr->data, data);
        new = w_list_append(new, new_data);
        ptr = w_list_next(ptr);
    }
    return new;
}

WList *w_list_sort(WList * list, WCompareFunc func)
{
    WList *l1, *l2;

    if (list == NULL)
        return NULL;
    l1 = w_list_first(list);
    if (l1->next == NULL)
        return l1;

    l2 = NULL;
    WList *p1, *p2;
    /* remove element from l1 one by one and add it into l2 */
    while (l1 != NULL) {
        p1 = p2 = l1;
        while (p1 != NULL) {
            if (func(p1->data, p2->data) < 0)   /* p1 should come before p2 */
                p2 = p1;
            p1 = w_list_next(p1);
        }
        l1 = w_list_remove(l1, p2->data);
        l2 = w_list_append(l2, p2->data);
    }

    return l2;
}

WList *w_list_concat(WList * l1, WList * l2)
{
    l1 = w_list_last(l1);
    l2 = w_list_first(l2);
    if (l1 == NULL)
        return l2;
    if (l2 == NULL)
        return w_list_first(l1);

    l1->next = l2;
    l2->prev = l1;
    return w_list_first(l1);
}

void w_list_foreach(WList * list, WFunc func, void *data)
{
    WList *ptr;
    ptr = w_list_first(list);
    while (ptr != NULL) {
        func(ptr->data, data);
        ptr = w_list_next(ptr);
    }
}

WList *w_list_first(WList * list)
{
    if (list == NULL)
        return NULL;
    while (list->prev != NULL) {
        list = w_list_prev(list);
    }
    return list;
}

WList *w_list_last(WList * list)
{
    if (list == NULL)
        return NULL;
    while (list->next != NULL) {
        list = w_list_next(list);
    }
    return list;
}

WList *w_list_nth(WList * list, unsigned int n)
{
    WList *ptr;
    ptr = w_list_first(list);
    unsigned int pos = 0;
    while (ptr != NULL) {
        if (pos == n)
            break;
        pos++;
        ptr = w_list_next(ptr);
    }
    return ptr;
}

void *w_list_nth_data(WList * list, unsigned int n)
{
    WList *ptr;
    ptr = w_list_nth(list, n);
    if (ptr == NULL)
        return NULL;
    return ptr->data;
}

WList *w_list_find(WList * list, const void *data)
{
    WList *ptr;
    ptr = w_list_first(list);
    while (ptr != NULL) {
        if (ptr->data == data)
            break;
        ptr = w_list_next(ptr);
    }
    return ptr;
}

WList *w_list_find_custom(WList * list, WCompareFunc func,
                          const void *data)
{
    WList *ptr;
    ptr = w_list_first(list);
    while (ptr != NULL) {
        if (func(ptr->data, (void *) data) == 0)
            break;
        ptr = w_list_next(ptr);
    }
    return ptr;
}

WList *w_list_alloc(void *data)
{
    WList *list;
    list = (WList *) malloc(sizeof(*list));
    list->data = data;
    list->prev = list->next = NULL;

    return list;
}
