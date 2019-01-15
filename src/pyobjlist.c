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

int pyobjlist_push_front(PyObjectListNode** head, PyObjectListNode** tail, PyObject* obj) {
	PyObjectListNode* node = (PyObjectListNode*)malloc(sizeof(PyObjectListNode));
	if (node == NULL)
		return 0;

	node->obj = obj;
	node->prev = NULL;
	node->next = *head;

	if (node->next) {
		node->next->prev = node;
	} else {
		*tail = node;
	}

	*head = node;

	return 1;
}

int pyobjlist_push_back(PyObjectListNode** head, PyObjectListNode** tail, PyObject* obj) {
	PyObjectListNode* node = (PyObjectListNode*)malloc(sizeof(PyObjectListNode));
	if (node == NULL)
		return 0;

	node->obj = obj;
	node->prev = *tail;
	node->next = NULL;

	if (node->prev) {
		node->prev->next = node;
	} else {
		*head = node;
	}

	*tail = node;

	return 1;
}

PyObject* pyobjlist_pop_front(PyObjectListNode** head, PyObjectListNode** tail) {
	PyObjectListNode* node = *head;
	if (!node)
		return NULL;

	if (node->next) {
		node->next->prev = NULL;
	} else {
		*tail = NULL;
	}

	*head = node->next;

	PyObject* result = node->obj;
	free(node);

	return result;
}

PyObject* pyobjlist_pop_back(PyObjectListNode** head, PyObjectListNode** tail) {
	PyObjectListNode* node = *tail;
	if (!node)
		return NULL;

	if (node->prev) {
		node->prev->next = NULL;
	} else {
		*head = NULL;
	}

	*tail = node->prev;

	PyObject* result = node->obj;
	free(node);

	return result;
}

void pyobjlist_clear(PyObjectListNode** head, PyObjectListNode** tail) {
	PyObject* current;
	while ((current = pyobjlist_pop_back(head, tail))) {
		Py_DECREF(current);
	}

	assert(*head == NULL);
	assert(*tail == NULL);
}

size_t pyobjlist_size(PyObjectListNode* head) {
	size_t count = 0;

	for (PyObjectListNode* node = head; node; node = node->next) {
		count++;
	}

	return count;
}
