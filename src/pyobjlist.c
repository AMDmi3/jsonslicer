/*
 * Copyright (c) 2019 Dmitry Marakasov <amdmi3@amdmi3.ru>
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 * THE SOFTWARE.
 */

#include "pyobjlist.h"

#include <stdlib.h>

void pyobjlist_init(PyObjList* list) {
	list->front = NULL;
	list->back = NULL;
}

void pyobjlist_clear(PyObjList* list) {
	PyObjListNode* cur = list->back;

	list->front = NULL;
	list->back = NULL;

	while (cur != NULL) {
		Py_DECREF(cur->obj);
		PyObjListNode* tmp = cur;
		cur = cur->prev;
		free(tmp);
	}
}

size_t pyobjlist_size(PyObjList* list) {
	size_t count = 0;

	for (PyObjListNode* node = list->front; node; node = node->next) {
		count++;
	}

	return count;
}

int pyobjlist_push_front(PyObjList* list, PyObject* obj) {
	PyObjListNode* node = (PyObjListNode*)malloc(sizeof(PyObjListNode));
	if (node == NULL)
		return 0;

	node->obj = obj;
	node->prev = NULL;
	node->next = list->front;

	if (node->next) {
		node->next->prev = node;
	} else {
		list->back = node;
	}

	list->front = node;

	return 1;
}

int pyobjlist_push_back(PyObjList* list, PyObject* obj) {
	PyObjListNode* node = (PyObjListNode*)malloc(sizeof(PyObjListNode));
	if (node == NULL)
		return 0;

	node->obj = obj;
	node->prev = list->back;
	node->next = NULL;

	if (node->prev) {
		node->prev->next = node;
	} else {
		list->front = node;
	}

	list->back = node;

	return 1;
}

PyObject* pyobjlist_pop_front(PyObjList* list) {
	PyObjListNode* node = list->front;
	if (!node)
		return NULL;

	if (node->next) {
		node->next->prev = NULL;
	} else {
		list->back = NULL;
	}

	list->front = node->next;

	PyObject* result = node->obj;
	free(node);

	return result;
}

PyObject* pyobjlist_pop_back(PyObjList* list) {
	PyObjListNode* node = list->back;
	if (!node)
		return NULL;

	if (node->prev) {
		node->prev->next = NULL;
	} else {
		list->front = NULL;
	}

	list->back = node->prev;

	PyObject* result = node->obj;
	free(node);

	return result;
}
