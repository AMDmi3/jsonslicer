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

#ifndef JSONSLICER_PYOBJLIST_H
#define JSONSLICER_PYOBJLIST_H

#include <Python.h>

typedef struct PyObjListNode {
	PyObject* obj;
	struct PyObjListNode* next;
	struct PyObjListNode* prev;
} PyObjListNode;

typedef struct PyObjList {
	PyObjListNode* front;
	PyObjListNode* back;
} PyObjList;

void pyobjlist_init(PyObjList* list);
void pyobjlist_clear(PyObjList* list);

size_t pyobjlist_size(PyObjList* list);
int pyobjlist_empty(PyObjList* list);

int pyobjlist_push_front(PyObjList* list, PyObject* obj);
int pyobjlist_push_back(PyObjList* list, PyObject* obj);

PyObject* pyobjlist_pop_front(PyObjList* list);
PyObject* pyobjlist_pop_back(PyObjList* list);

PyObject* pyobjlist_as_tuple_prefix(PyObjList* list, PyObject* obj);

#endif
