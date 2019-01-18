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

#include "output_formats.h"

#include "pyobjlist.h"
#include "pymutindex.h"

PyObject* generate_output_object(JsonSlicer* self, PyObject* obj) {
	if (self->path_mode == PATHMODE_DROP) {
		Py_INCREF(obj);
		return obj;
	} else if (self->path_mode == PATHMODE_MAP_KEYS) {
		PyObject* path_last = pyobjlist_back(&self->path);
		if (PyBytes_Check(path_last)) {
			PyObject* tuple = PyTuple_New(2);
			if (tuple == NULL) {
				return NULL;
			}
			Py_INCREF(path_last);
			PyTuple_SET_ITEM(tuple, 0, path_last);
			Py_INCREF(obj);
			PyTuple_SET_ITEM(tuple, 1, obj);
			return tuple;
		} else {
			Py_INCREF(obj);
			return obj;
		}
	} else if (self->path_mode == PATHMODE_FULL) {
		PyObject* tuple = PyTuple_New(pyobjlist_size(&self->path) + 1);
		if (tuple == NULL) {
			return NULL;
		}

		size_t tuple_idx = 0;
		for (PyObjListNode* node = self->path.front; node; node = node->next) {
			if (PyMutIndex_Check(node->obj)) {
				PyObject* index = PyMutIndex_AsPyLong(node->obj);
				if (index == NULL) {
					Py_DECREF(tuple);
					return NULL;
				}
				PyTuple_SET_ITEM(tuple, tuple_idx++, index);
			} else {
				Py_INCREF(node->obj);
				PyTuple_SET_ITEM(tuple, tuple_idx++, node->obj);
			}
		}

		Py_INCREF(obj);
		PyTuple_SET_ITEM(tuple, tuple_idx, obj);

		return tuple;
	} else {
		PyErr_SetString(PyExc_RuntimeError, "Unexpected path mode");
		return NULL;
	}
}
