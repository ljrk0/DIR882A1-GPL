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
        A simple thread safe queue.

        Author: bhoover@wecs.com
        Date: March 2015

        History:
                - first implemetnation
*/

#include <stdlib.h>
#include "queue.h"
#include "list.h"
#include "threads_wrapper.h"
#include "safe_mem.h"

#define MODULE_TAG	"QUEUE: "

typedef struct simple_queue {

	gen_list	a_queue;
	mutex_t		q_mutex;
}simple_queue;

void queue_enqueue(mt_queue_t a_queue,void *item)
{

	get_mutex(&((simple_queue*) a_queue)->q_mutex);

	/*note, assumes list_add function adds to list end
	*/
	list_add(((simple_queue*) a_queue)->a_queue,item);

	release_mutex(&((simple_queue*) a_queue)->q_mutex);
}

void* queue_dequeue(mt_queue_t a_queue)
{
	void*		item=0;
	gen_list_itr	itr=list_get_itr(((simple_queue*) a_queue)->a_queue);


	get_mutex(&((simple_queue*) a_queue)->q_mutex);

	if (list_count(((simple_queue*) a_queue)->a_queue)) {

		list_begin(((simple_queue*) a_queue)->a_queue,itr);

		item=list_entry_remove(((simple_queue*) a_queue)->a_queue,itr);
	}

	release_mutex(&((simple_queue*) a_queue)->q_mutex);


	return item;
}

int queue_size(mt_queue_t a_queue)
{

	int	queue_items;


	get_mutex(&((simple_queue*) a_queue)->q_mutex);

	queue_items=list_count(((simple_queue*) a_queue)->a_queue);

	release_mutex(&((simple_queue*) a_queue)->q_mutex);


	return queue_items;
}

mt_queue_t queue_create(mt_queue_t *a_queue)
{

	*a_queue=safe_malloc(sizeof(simple_queue));

	list_create(&((simple_queue*) *a_queue)->a_queue);

	create_mutex(&((simple_queue*) *a_queue)->q_mutex);


	return *a_queue;
}

void queue_destroy(mt_queue_t *a_queue)
{
	gen_list_itr	itr=list_get_itr(((simple_queue*) *a_queue)->a_queue);


	destroy_mutex(&((simple_queue*) *a_queue)->q_mutex);


	list_begin(((simple_queue*) *a_queue)->a_queue,itr);

	while (!(list_end(itr)))

		free(list_entry_remove(((simple_queue*) *a_queue)->a_queue,itr));

	list_destroy(&((simple_queue*) *a_queue)->a_queue);

	free(*a_queue);

	*a_queue=0;
}
