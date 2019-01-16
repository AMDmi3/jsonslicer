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

#ifndef JSONSLICER_GENERATOR_H
#define JSONSLICER_GENERATOR_H

#include "pyobjlist.h"

#include <Python.h>
#include <yajl/yajl_parse.h>

enum JsonSlicerMode {
	MODE_SEEKING,
	MODE_CONSTRUCTING
};

typedef struct {
	PyObject_HEAD

	PyObject* io;
	yajl_handle yajl;

	PyObject* last_map_key;
	int mode;

	// pattern argument
	PyObjectListNode* pattern_head;
	PyObjectListNode* pattern_tail;

	// current path in json
	PyObjectListNode* path_head;
	PyObjectListNode* path_tail;

	// stack of objects being currently constructed
	PyObjectListNode* constructing_head;
	PyObjectListNode* constructing_tail;

	// complete python objects ready to be returned to caller
	PyObjectListNode* complete_head;
	PyObjectListNode* complete_tail;
} JsonSlicer;

PyTypeObject JsonSlicerType;

#endif
