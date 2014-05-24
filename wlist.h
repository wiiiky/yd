/*=============================================================================
#     FileName: wlist.h
#         Desc: double linked list
#       Author: Wiky L
#        Email: wiiiky@yeah.net
#     HomePage: https://launchpad.net/~wiiiky-v
#      Version: 0.0.1
#   LastChange: 2013-09-21 10:00:23
#      History:
=============================================================================*/

#ifndef _W_LIST_H
#define _W_LIST_H

/* 通用的双向链表 */
typedef struct _WList WList;
struct _WList {
	WList *prev;
	WList *next;
	void *data;
};

/* 
 * the function to compare elements int the list.
 * It should return a number >0 if a comes after
 * b in the sort order
 */
typedef int (*WCompareFunc) (void *a, void *b);
/*
 * Specify the type of function which is called
 * when a data element is destroyed. It's passed
 * the pointer to the data element, and should free
 * all memory and resources acllocated for it
 */
typedef void (*WDestroyNotify) (void *data);
/*
 * a function for copying data
 */
typedef void *(*WCopyFunc) (const void *data, void *user_data);
/*
 * Specify the type of function passed to 
 * w_list_foreach and
 */
typedef void (*WFunc) (void *data, void *user_data);




/* append data to the end of list */
WList *w_list_append(WList * list, void *data);
/* insert data before the header of list */
WList *w_list_prepend(WList * list, void *data);
/*
 * position starts from zero
 * if position <= 0, data is inserted before header of list
 * if position >= the length of list, data is appended to the end of list
 */
WList *w_list_insert(WList * list, void *data, unsigned int position);
/*
 * Insert a new element into the list,using the giving comparision
 * function to determine its position
 */
WList *w_list_insert_sorted(WList * list, WCompareFunc func, void *data);
/*
 * Removes an elements from a list.
 * Only one element that contains data will be removed
 * If no element found, no change
 */
WList *w_list_remove(WList * list, const void *data);
/*
 * remove all elements that match the given data
 * the 'ALL' version of w_list_remove
 */
WList *w_list_remove_all(WList * list, const void *data);

/* 
 * free all of memory used by a WList,
 * but if the elements contain dynamically-allocated memory
 * won't be freed.
 */
void w_list_free(WList * list);
/*
 * free all of memory used by a Wlist,
 * free all dynamically-allocated memory contained by elements,
 * using the given destroy function
 */
void w_list_free_full(WList * list, WDestroyNotify func);
/* return the number of elements in list */
unsigned int w_list_length(WList * list);

/*
 * copy a list
 * If the list element consist of pointers to data,
 * the pointer is copied but the actual data is not
 */
WList *w_list_copy(WList * list);
/* make a full copy fo a list */
WList *w_list_copy_deep(WList * list, WCopyFunc func, void *data);

/* sort a list using the given comparison function */
WList *w_list_sort(WList * list, WCompareFunc func);

/* just add the second list onto the end of first one */
WList *w_list_concat(WList * list1, WList * list2);
/* call the given function for each element of list */
void w_list_foreach(WList * list, WFunc func, void *data);


/* get the first and the last element of list */
WList *w_list_first(WList * list);
WList *w_list_last(WList * list);
/* get the previous and the next element of current element */
#define w_list_prev(list) list->prev
#define w_list_next(list) list->next

/* 
 * get the element at the given position
 * if n >= the length of list
 * return NULL
 */
WList *w_list_nth(WList * list, unsigned int n);
/* get the data of element at the given position */
void *w_list_nth_data(WList * list, unsigned int n);

/* return the element that contains the given data */
WList *w_list_find(WList * list, const void *data);
/*
 * find an element in a list,using a supplied function
 * to find the desired element. It iterates over the list,
 * calling the given function which should return 0 when
 * the desired element is found
 */
WList *w_list_find_custom(WList * list, WCompareFunc func,
						  const void *data);

/* return a new element */
WList *w_list_alloc(void *data);

#endif
