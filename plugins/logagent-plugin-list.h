/*
 * Copyright (c) 2018 Steven Lee <geekerlw@gmail.com>
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 */

#ifndef __LOGAGENT_PLUGIN_LIST_H__
#define __LOGAGENT_PLUGIN_LIST_H__

#ifndef offsetof
#define offsetof(type, member)  (size_t)(&((type*)0)->member)
#endif

#define container_of(ptr, type, member) (type *)((char*)(ptr) - offsetof(type, member))

struct list_head {
	struct list_head *next, *prev;
};

#define LIST_HEAD_INIT(name)    {&(name), &(name)}

#define LIST_HEAD(name) struct list_head name = LIST_HEAD_INIT(name)

/**
* list_entry - get the struct for this entry
* @ptr:	the &struct list_head pointer.
* @type:	the type of the struct this is embedded in.
* @member:	the name of the list_head within the struct.
*/
#define list_entry(ptr, type, member) \
	container_of(ptr, type, member)

/**
* list_first_entry - get the first element from a list
* @ptr:	the list head to take the element from.
* @type:	the type of the struct this is embedded in.
* @member:	the name of the list_head within the struct.
*
* Note, that list is expected to be not empty.
*/
#define list_first_entry(ptr, type, member) \
	list_entry((ptr)->next, type, member)

/**
* list_next_entry - get the next element in list
* @pos:	the type * to cursor
* @member:	the name of the list_head within the struct.
*/
#define list_next_entry(pos, type, member) \
	list_entry((pos)->member.next, type, member)

/**
* list_for_each	-	iterate over a list
* @pos:	the &struct list_head to use as a loop cursor.
* @head:	the head for your list.
*/
#define list_for_each(pos, head) \
	for (pos = (head)->next; pos != (head); pos = pos->next)

/**
* list_for_each_safe - iterate over a list safe against removal of list entry
* @pos:	the &struct list_head to use as a loop cursor.
* @n:		another &struct list_head to use as temporary storage
* @head:	the head for your list.
*/
#define list_for_each_safe(pos, n, head) \
	for (pos = (head)->next, n = pos->next; pos != (head); \
		pos = n, n = pos->next)

/**
* list_for_each_entry	-	iterate over list of given type
* @pos:	the type * to use as a loop cursor.
* @head:	the head for your list.
* @member:	the name of the list_head within the struct.
*/
#define list_for_each_entry(pos, type, head, member)				\
	for (pos = list_first_entry(head, type, member);	\
	     &pos->member != (head);					\
	     pos = list_next_entry(pos, type, member))

/**
* list_for_each_entry_safe - iterate over list of given type safe against removal of list entry
* @pos:	the type * to use as a loop cursor.
* @n:		another type * to use as temporary storage
* @head:	the head for your list.
* @member:	the name of the list_head within the struct.
*/
#define list_for_each_entry_safe(pos, type, n, head, member)			\
	for (pos = list_first_entry(head, type, member),	\
		n = list_next_entry(pos, type, member);			\
	     &pos->member != (head); 					\
	     pos = n, n = list_next_entry(n, type, member))

static inline void list_init(struct list_head *list)
{
	list->next = list;
	list->prev = list;
}

static inline int list_empty(struct list_head *list)
{
	return list->next == list;
}

static inline void
list_insert(struct list_head *link, struct list_head *new_link)
{
	new_link->prev = link->prev;
	new_link->next = link;
	new_link->prev->next = new_link;
	new_link->next->prev = new_link;
}

static inline void
list_append(struct list_head *list, struct list_head *new_link)
{
	list_insert((struct list_head *)list, new_link);
}

static inline void list_remove(struct list_head *link)
{
	link->prev->next = link->next;
	link->next->prev = link->prev;
}

#endif /* __LOGAGENT_PLUGIN_LIST_H__ */
