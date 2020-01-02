/*
Copyright (C) 2015 Bryan Hoover (bhoover@wecs.com)

This program is free software; you can redistribute it and/or
modify it under the terms of the GNU General Public License
as published by the Free Software Foundation; either version 3
of the License, or (at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program; if not, write to the Free Software
Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.
*/

/*
	Singly linked list.

	Author: bhoover@wecs.com
	Date: Feb 2015

	History:
		- first implemetnation
*/

#include <stdlib.h>
#include <string.h>
#include "list.h"
#include "safe_mem.h"

#define MODULE_TAG	"LIST: "

typedef struct entry {

	void*		item;
	struct entry*	next;
}entry;

typedef struct iterator {

	void*	entry;
	void*	prev;

	/*true if this instance created,
	  and returned by list_create
	*/
	int	is_list_created;
}iterator;

typedef struct list {

	entry*		head;
	entry*		last;

	gen_list_itr	itr;

	int		count;
}list;

void list_add(gen_list a_list,void* item)
{
	list*	this_list=a_list->a_list;


	if (!(this_list))

		return;

	if (this_list->count) {

		this_list->last->next=safe_malloc(sizeof(entry));

		this_list->last->next->item=item;

		this_list->last=this_list->last->next;
	}
	else {
		this_list->head=safe_malloc(sizeof(entry));

		this_list->head->item=item;

		this_list->last=this_list->head;

		((iterator*) this_list->itr->a_itr)->entry=this_list->head;
	}

	this_list->count++;
}

static entry* get_entry(list* this_list,entry** prev,void* key_data,gen_list_cb is_sought_entry)
{
	entry*	cur_entry;


	if (!(this_list))

		return 0;

	if (!(cur_entry=this_list->head))

		return 0;

	*prev=0;

	while(cur_entry) {

		if (is_sought_entry(cur_entry->item,key_data))

			break;

		*prev=cur_entry;

		cur_entry=cur_entry->next;
	}

	return cur_entry;
}

/*gen_list_cb example for client implementation
static gen_list_cb is_sought_entry(void *item,void *key)
{

	return 0;
}
*/
void* list_item(gen_list a_list,void* key_data,gen_list_cb is_sought_entry)
{
	entry*		prev;
	entry*		this_entry;

	if (!(this_entry=get_entry((list*) a_list->a_list,&prev,key_data,is_sought_entry)))

		return 0;

	return this_entry->item;
}

static void* remove_entry(list* this_list,entry* found,entry* prev)
{
	void*	found_item;

	/*not case of found is first list item (head)*/
	if (!(found==this_list->head)) {

		/*not case of found is last list item*/
		if (found->next) {

			prev->next=found->next;
		}
		else {

			this_list->last=prev;
			this_list->last->next=0;
		}
	}
	else {
		/*not case of found is last list item (emptying list)*/
		if (found->next) {

			this_list->head=found->next;
		}
		else {

			this_list->head=0;
			this_list->last=0;
		}
	}

	found_item=found->item;

	free(found);

	this_list->count--;

	return found_item;
}

void* list_entry_remove(gen_list a_list,gen_list_itr itr)
{
	entry*	cur_entry=((iterator *) itr->a_itr)->entry;


	((iterator *) itr->a_itr)->entry=((entry *) ((iterator *) itr->a_itr)->entry)->next;

	return remove_entry((list *) a_list->a_list,cur_entry,((iterator *) itr->a_itr)->prev);
}

void* list_remove(gen_list a_list,void* key_data,gen_list_cb is_sought_entry)
{
	list*	this_list=a_list->a_list;
	entry*	found;
	entry*	prev;


	if (!(found=get_entry(this_list,&prev,key_data,is_sought_entry)))

		return 0;

	return remove_entry(this_list,found,prev);
}

gen_list_itr list_begin(gen_list a_list,gen_list_itr itr)
{

	((iterator *) itr->a_itr)->entry=((list *) a_list->a_list)->head;

	((iterator *) itr->a_itr)->prev=0;

	return itr;
}

gen_list_itr list_itr_create(gen_list a_list,gen_list_itr* itr)
{

	*itr=safe_malloc(sizeof(_gen_list_itr));

	(*itr)->a_itr=safe_malloc(sizeof(iterator));

	list_begin(a_list,*itr);


	return *itr;
}

void list_itr_destroy(gen_list_itr* itr)
{
	iterator*	this_itr=(iterator *) (*itr)->a_itr;

	if (!(this_itr->is_list_created)) {

		free(this_itr);
		free(*itr);

		*itr=0;
	}
}

void list_itr_set(gen_list_itr	itr_dest,gen_list_itr itr_src)
{

	memcpy(itr_dest->a_itr,itr_src->a_itr,sizeof(iterator));
}

gen_list_itr list_next(gen_list_itr itr)
{

	((iterator *) itr->a_itr)->prev=((iterator *) itr->a_itr)->entry;

	((iterator *) itr->a_itr)->entry=((entry *) ((iterator *) itr->a_itr)->entry)->next;


	return itr;
}

int list_end(gen_list_itr itr)
{

	return (((iterator *) itr->a_itr)->entry==0);
}

void* list_entry_item(gen_list_itr itr)
{

	return ((entry *) ((iterator *) itr->a_itr)->entry)->item;
}

int list_count(gen_list a_list)
{
	list*	this_list=a_list->a_list;

	if (!(this_list))

		return 0;

	return this_list->count;
}

gen_list_itr list_get_itr(gen_list a_list)
{

	return ((list *)  a_list->a_list)->itr;
}

/*pointer to gen_list_itr argument at client option as this function returns gen_list_itr, and
  list_destroy free same.
*/
gen_list_itr list_create(gen_list* a_list)
{

	*a_list=safe_malloc(sizeof(_gen_list));
	(*a_list)->a_list=safe_malloc(sizeof(list));
	((list *) (*a_list)->a_list)->itr=safe_malloc(sizeof(_gen_list_itr));
	((list *) (*a_list)->a_list)->itr->a_itr=safe_malloc(sizeof(iterator));

	((iterator *) ((list *) (*a_list)->a_list)->itr->a_itr)->is_list_created=1;

	list_begin((*a_list),((list *) (*a_list)->a_list)->itr);

	return ((list *) (*a_list)->a_list)->itr;
}

void list_destroy(gen_list* a_list)
{
	list*		this_list=(list *) ((*a_list)->a_list);


	if (!(this_list))

		return;

	while (this_list->count) {

		remove_entry(this_list,this_list->head,this_list->head);
	}

	free(this_list->itr->a_itr);
	free(this_list->itr);

	free(this_list);
	free(*a_list);

	*a_list=0;
}
