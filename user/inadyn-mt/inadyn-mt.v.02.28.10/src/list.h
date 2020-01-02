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

#ifndef _GEN_LIST_H_INCLUDED
#define _GEN_LIST_H_INCLUDED

typedef struct _gen_list {

	void	*a_list;

} _gen_list;

typedef struct _gen_list_itr {

	void	*a_itr;

} _gen_list_itr;

typedef _gen_list*	gen_list;
typedef _gen_list_itr*	gen_list_itr;

/*	int gen_list_cmp(void*,void*)
	{
		..

		return int
	}
*/

typedef int (*gen_list_cb) (void* item,void* key);

/*list_create return value destroyed on list_destroy
*/
gen_list_itr	list_create(gen_list* a_list);
void		list_destroy(gen_list* a_list);
void 		list_add(gen_list a_list,void* item);
void*		list_item(gen_list a_list,void* key_data,gen_list_cb is_sought_entry_cb);
void*		list_remove(gen_list a_list,void* key_data,gen_list_cb is_sought_entry_cb);
gen_list_itr	list_get_itr(gen_list a_list);
int		list_count(gen_list a_list);
gen_list_itr	list_itr_create(gen_list a_list,gen_list_itr* itr);
void		list_itr_destroy(gen_list_itr* itr);
void		list_itr_set(gen_list_itr itr_dest,gen_list_itr itr_src);
/*iterator functions must NOT be passed null pointer*/
gen_list_itr	list_begin(gen_list a_list,gen_list_itr itr);
/*do NOT iterate, or attempt access past list - use list_end to detect, and list_begin to reset*/
gen_list_itr	list_next(gen_list_itr itr);
int		list_end(gen_list_itr itr);
void*		list_entry_item(gen_list_itr itr);
void*		list_entry_remove(gen_list a_list,gen_list_itr itr);

#endif
