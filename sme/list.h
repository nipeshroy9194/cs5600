/** 
 * GNU_SOURCE
 * Reference from Samba Library a doubly linked list library
 */

#ifndef _LIST_H_
#define _LIST_H_

/*
   add an element at the front of a head
*/
#define LIST_ADD(head, new_el) \
do { \
        if (!(head)) { \
		(new_el)->prev = (head) = (new_el);  \
		(new_el)->next = NULL; \
	} else { \
		(new_el)->prev = (head)->prev; \
		(head)->prev = (new_el); \
		(new_el)->next = (head); \
		(head) = (new_el); \
	} \
} while (0)

/**
 * remove an element from a list
 */
#define LIST_REMOVE(head, new_el) \
do { \
	if ((new_el) == (head)) { \
		if ((new_el)->next) (new_el)->next->prev = (new_el)->prev; \
		(head) = (new_el)->next; \
	} else if ((head) && (new_el) == (head)->prev) {	\
		(new_el)->prev->next = NULL; \
		(head)->prev = (new_el)->prev; \
	} else { \
		if ((new_el)->prev) (new_el)->prev->next = (new_el)->next; \
		if ((new_el)->next) (new_el)->next->prev = (new_el)->prev; \
	} \
	if ((new_el) != (head)) (new_el)->next = (new_el)->prev = NULL;	\
} while (0)

/**
 * insert 'p' after the given element 'given_el' in a head.
 * If given_el is NULL then
 * this is the same as a LIST_ADD()
 */
#define LIST_ADD_AFTER(head, new_el, given_el) \
do { \
	if (!(head) || !(given_el)) { \
		LIST_ADD(head, new_el); \
	} else { \
		(new_el)->prev = (given_el);   \
		(new_el)->next = (given_el)->next;		\
		(given_el)->next = (new_el);		\
		if ((new_el)->next) (new_el)->next->prev = (new_el);	\
		if ((head)->prev == (given_el)) (head)->prev = (new_el); \
	}\
} while (0)

/**
 * add to the end of a list.
 */
#define LIST_ADD_END(head, new_el) \
do { \
	if (!(head)) { \
		LIST_ADD(head, new_el); \
	} else { \
		LIST_ADD_AFTER(head, new_el, (head)->prev); \
	} \
} while (0)

#endif /* _LIST_H */
