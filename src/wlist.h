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

#ifndef __W_WLIST_H__
#define __W_WLIST_H__
/*
 * WList : double linked list
 */
typedef struct _WList WList;

struct _WList {
    WList *prev;
    WList *next;
    void *data;
};

/*
 * @description: creates a new element with data
 * 
 * @param data: the data for the new element
 * 
 * @return: a new allocated element
 */
WList *w_list_alloc(void *data);


/*
 * @description: frees the memory of list element
 */
void w_list_free1(WList * list);



/*
 * @description: the type of function passed to w_list_free_full
 * 
 * @param data: the data of element
 */
typedef void (*WListDestroy) (void *data);
void w_list_free1_full(WList * list, WListDestroy destroy);

/*
 * @description: creates a new list without any element.
 * 
 * @return: NULL.
 */
WList *w_list_new();


/*
 * @description: gets the first/last element of list
 * 
 * @param list: not NULL
 */
WList *w_list_first(WList * list);
WList *w_list_last(WList * list);


/*
 * @description: the previous/next element
 * @param list: not NULL
 */
#define w_list_next(list)   ((list)->next)
#define w_list_prev(list)   ((list)->prev)

#define w_list_data(list)   ((list)->data)

/*
 * @description: appends a new element with data to the end of the list.
 * 
 * @param list: a pointer to a WList.
 * @param data: the data for the new element.
 *
 * @return: the new start of the list.
 */
WList *w_list_append(WList * list, void *data);

/*
 * @description: inserts a new element with data to the list 
 *				at the given position.
 * 
 * @param list: a pointer to a WList.
 * @param data: the data for the new element.
 * @param position: the position to insert.
 *					if position <0, data is append to the end of list.
 *					if position >=0, data is inserted into the position of list.
 * 
 * @return: the new start of the list
 */
WList *w_list_insert(WList * list, void *data, int position);

/*
 * @description: The type of function passed to w_list_foreach
 * 
 * @param data: the data of element
 * @param user_data: user data passed to w_list foreach
 */
typedef void (*WForeachFunc) (void *data, void *user_data);
/*
 * @description: calls function func for each element in the list
 * 
 * @param list: the list
 * @param func: the function to call for each element's data
 * @param user_data: the user data passed to the function
 */
void w_list_foreach(WList * list, WForeachFunc func, void *user_data);

/*
 * @description: frees a list
 */
void w_list_free(WList * list);

/*
 * @description: frees a list,and call destroy on every element
 * 
 * @param list: the list
 * @param destroy: the destroy function
 */
void w_list_free_full(WList * list, WListDestroy destroy);


/*
 * @description: the function to compare data
 * 
 * @return: if a > b, return positive,
 *			if a == b, return zero,
 *			if a < b, return negative
 */
typedef int (*WCompareFunc) (const void *a, const void *b);
/*
 * @description: finds the element with data that equals to b
 * 
 * @return: NULL if not found
 */
WList *w_list_find_custom(WList * list, WCompareFunc func, const void *b);

/*
 * @description: sorts a list using bubble sort.
 *               return the new head of list
 * 
 * @param list: the list to sort
 * @param func: the function to compare two elements.
 * 
 * @return: the new head of list
 */
WList *w_list_sort_bubble(WList * list, WCompareFunc func);


/*
 * @description: sorts a list using insertion sort
 * 
 * @param list: the list to sort
 * @param func: the function to compare two elements.
 * 
 * @return: the new head of list;
 */
WList *w_list_sort_insertion(WList * list, WCompareFunc func);

/*
 * use insertion sort by default
 */
#define w_list_sort(list,func)  w_list_sort_insertion (list,func)

/*
 * @description: inserts data into list, and use given compare function to 
 *              determine position.
 *
 * @param list: the list
 * @param data: the data to insert
 * @param func: the compare function
 * 
 * @return: the new head of list
 */
WList *w_list_insert_sorted(WList * list, void *data, WCompareFunc func);


/*
 * @description: finds the element in the given list that contains data.
 * 
 * @return: the element or NULL if fail.
 */
WList *w_list_find(WList * list, void *data);


/*
 * @description: removes an element from list
 *				 this function does not free data.
 * 
 * @param data: the data of element
 * 
 * @return: the new start of list
 */
WList *w_list_remove(WList * list, void *data);

/*
 * removes and frees data
 */
WList *w_list_remove_full(WList * list, void *data, WListDestroy destroy);

/*
 * @description: reverses a list
 * 
 * @param list: the list to reserse
 *
 * @return: the new head of list
 */
WList *w_list_reverse(WList * list);


#endif
