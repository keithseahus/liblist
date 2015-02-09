/*
 *
 *  liblist
 *
 *    - Author: Keisuke TAKAHASHI <keithseahus &#64 gmail.com>
 *    - Site: https://github.com/keithseahus/liblist
 *
 *    Licensed under the Apache License, Version 2.0 (the "License");
 *    you may not use this file except in compliance with the License.
 *    You may obtain a copy of the License at
 *
 *        http://www.apache.org/licenses/LICENSE-2.0
 *
 *    Unless required by applicable law or agreed to in writing, software
 *    distributed under the License is distributed on an "AS IS" BASIS,
 *    WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 *    See the License for the specific language governing permissions and
 *    limitations under the License.
 *
*/

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <assert.h>

#include "list.h"


/*
 * List Object
 */

static int List_join(List *self, List *target);
static int List_add(List *self, List *target);
static int List_set_tag(List *self, int tag);
static int List_add_tag(List *self, int tag);
static int List_add_with_tag(List *self, void *target, int tag);
static int List_terminate(List *self);
static int List_dump(List *self, List *list);
static int List_foreach(List *self, void *function, void *arg);
static int List_length(List *self);
static int List_initialize(List *self);
static int List_destroy(List *self);


static int List_join(List *self, List *target) {
	if (!self || self->prev || !target) goto err;

	List *ptr;
	List *ptr_prev;

	ptr = self;

	if (ptr->prev) goto err;

	while (ptr->next) {
		ptr_prev = ptr;
		ptr = ptr->next;
	}

	ptr->next = target;

	ptr = ptr->next;
	ptr->prev = self;
	return LIBLIST_RETVAL_SUCCESS;

err:
	printf("self->prev should be NULL.\n");
	return LIBLIST_RETVAL_FAILED;
}

static int List_add(List *self, List *target) {
	if (!self || self->prev || !target) goto err;

	int ret = 0;

	List list = ListElements;
	list.initialize(&list);
	memcpy(list.data, &target, sizeof(target));
	ret = self->join(self, &list);
	if (ret != LIBLIST_RETVAL_SUCCESS) goto err;

	return LIBLIST_RETVAL_SUCCESS;

err:
	printf("self->prev should be NULL.\n");
	return LIBLIST_RETVAL_FAILED;
}

static int List_set_tag(List *self, int tag) {
	self->tag = tag;
	return LIBLIST_RETVAL_SUCCESS;
}

static int List_add_tag(List *self, int tag) {
	if (!self || self->prev) goto err;

	List *ptr;

	ptr = self;

	while(ptr->next) ptr = ptr->next;

	self->set_tag(self, tag);
	return LIBLIST_RETVAL_SUCCESS;

err:
	printf("self->prev should be NULL.\n");
	return LIBLIST_RETVAL_FAILED;
}

static int List_add_with_tag(List *self, void *target, int tag) {
	if (!self || self->prev || !target) goto err;

	int ret = 0;

	ret = self->add(self, target);
	if (ret != LIBLIST_RETVAL_SUCCESS) goto err;

	ret = self->add_tag(self, tag);
	if (ret != LIBLIST_RETVAL_SUCCESS) goto err;

	return LIBLIST_RETVAL_SUCCESS;

err:
	printf("self->prev should be NULL.\n");
	return LIBLIST_RETVAL_FAILED;
}

static int List_terminate(List *self) {
	if (!self || self->prev) goto err;

	int ret = 0;

	void *buf = malloc(sizeof(List));
	List list = ListElements;
	memcpy(buf, &list, sizeof(list));
	ret = self->join(self, &list);
	list.destroy(&list);
	if (ret != LIBLIST_RETVAL_SUCCESS) goto err;

	return LIBLIST_RETVAL_SUCCESS;

err:
	printf("self->prev should be NULL.\n");
	return LIBLIST_RETVAL_FAILED;
}

static int List_dump(List *self, List *list) {
	if (!self || !list) goto err;

	printf("list(%p)->data(%p) is %s\n", list, list->data, (char *)list->data);
	printf("list(%p)->prev is %p\n", list, list->prev);
	printf("list(%p)->next is %p\n", list, list->next);
	return LIBLIST_RETVAL_SUCCESS;

err:
	printf("list should not be NULL.\n");
	return LIBLIST_RETVAL_FAILED;
}

static int List_foreach(List *self, void *function, void *arg) {
	if (!self || self->prev || !function) goto err;

	List *ptr;
	ptr = self;

	while (ptr->next) {
		if(!arg) {
			LibListFunc_0 *fun = function;
			(*fun)(self, ptr);
		} else {
			LibListFunc_1 *fun = function;
			(*fun)(self, ptr, arg);
		}
		ptr = ptr->next;
	}

	return LIBLIST_RETVAL_SUCCESS;

err:
	printf("self->prev should be NULL.\n");
	return LIBLIST_RETVAL_FAILED;
}

static int List_length(List *self) {
	if (!self || self->prev) goto err;

	List *ptr = self;
	int len = 1;

	for (len; ptr->next; len++) {
		ptr = ptr->next;
	}

	return len;

err:
	printf("self->prev should be NULL.\n");
	return -1;
}

static int List_initialize(List *self) {
	if (!self) goto err;

	self->data = malloc(sizeof(void*));
	memset(self->data, 0, sizeof(void*));
	return LIBLIST_RETVAL_SUCCESS;

err:
	printf("list should not be NULL.\n");
	return LIBLIST_RETVAL_FAILED;
}

static int List_destroy(List *self) {
	if (!self) goto err;

	if(self->data) free(self->data);
	if(self->next) free(self->next);
	memset(self, 0, sizeof(List));
	return LIBLIST_RETVAL_SUCCESS;

err:
	printf("list should not be NULL.\n");
	return LIBLIST_RETVAL_FAILED;
}


/*
 * ListHelper Object
 */

// TODO: Delete all the list recursively.
static int ListHelper_destroy_list(ListHelper *self, List *list) {
	if (!self || !list) goto err;

	List *ptr = self->last(self, list);
	while (ptr->prev) {
		ptr = ptr->prev;
		ptr->destroy(ptr->next);
	}
	ptr->destroy(ptr);
	return LIBLIST_RETVAL_SUCCESS;

err:
	printf("list should not be NULL.\n");
	return LIBLIST_RETVAL_FAILED;
}

static List *ListHelper_new_list(ListHelper *self) {
	if (!self) goto err;

	void *buf = malloc(sizeof(List));
	List list = ListElements;
	memcpy(buf, &list, sizeof(list));
	return buf;

err:
	printf("self should not be NULL.\n");
	return NULL;
}

static List *ListHelper_last(ListHelper *self, List *list) {
	if (!self || !list) goto err;

	List *ptr;
	ptr = list;
	while (ptr->next) {
		ptr = ptr->next;
	}

	return ptr;

err:
	printf("self should not be NULL.\n");
	return NULL;
}

static List *ListHelper_find_by_tag(ListHelper *self, List *list, int tag) {
	if (!self || !list) goto err;

	List *ptr;
	ptr = list;

	while (ptr->next) {
		if(ptr->tag == tag) break;
		ptr = ptr->next;
		continue;
	}
	return ptr;

err:
	printf("self should not be NULL.\n");
	return NULL;
}

static List *ListHelper_reverse(ListHelper *self, List *list) {
	if (!self || !list || list->prev) goto err;

	List *list_next;
	List *list_prev;

	do {
		list_prev  = list->prev;
		list_next  = list->next;
		list->next = list_prev;
		list->prev = list_next;
		if (list->prev) list = list->prev;
	} while (list->prev);

	return list;

err:
	return NULL;
}

static int ListHelper_destroy(ListHelper *self) {
	if (!self) goto err;

	memset(self, 0, sizeof(ListHelper));
	free(self);
	return LIBLIST_RETVAL_SUCCESS;

err:
	printf("self should not be NULL.\n");
	return LIBLIST_RETVAL_FAILED;
}

ListHelper *newListHelper() {
	void *buf = malloc(sizeof(ListHelper));
	if (!buf) goto err;

	ListHelper list_helper = ListHelperElements;
	memcpy(buf, &list_helper, sizeof(list_helper));
	return buf;

err:
	return NULL;
}

