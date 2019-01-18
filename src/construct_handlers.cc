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

#include "construct_handlers.hh"

#include "seek_handlers.hh"

#include "pyobjlist.hh"

#include <Python.h>

#include <assert.h>

// helpers
int add_to_parent(JsonSlicer* self, PyObject* value) {
	PyObject* container = self->constructing.back->obj;

	if (PyDict_Check(container)) {
		if (!PyBytes_Check(self->last_map_key)) {
			PyErr_SetString(PyExc_RuntimeError, "No map key available");
			return 0;
		}

		// adds references
		if (PyDict_SetItem(container, self->last_map_key, value) != 0) {
			return 0;
		}
	} else if (PyList_Check(container)) {
		// adds reference
		return PyList_Append(container, value) == 0;
	} else {
		PyErr_SetString(PyExc_RuntimeError, "Unexpected container type on the stack");
		return 0;
	}

	return 1;
}

static int push_constructing_object(JsonSlicer* self, PyObject* obj) {
	if (!pyobjlist_empty(&self->constructing)) {
		if (!add_to_parent(self, obj)) {
			return 0;
		}
	}

	return pyobjlist_push_back(&self->constructing, obj);
}

static PyObject* pop_constructing_object(JsonSlicer* self) {
	return pyobjlist_pop_back(&self->constructing);
}

// containers
int construct_handle_start_map(JsonSlicer* self) {
	PyObject* map = PyDict_New();
	if (!map) {
		return 0;
	}

	if (!push_constructing_object(self, map)) {
		Py_DECREF(map);
		return 0;
	}
	return 1;
}

int construct_handle_end_map(JsonSlicer* self) {
	PyObject* map = pop_constructing_object(self);

	if (pyobjlist_empty(&self->constructing)) {
		if (!finish_complete_object(self, map)) {
			Py_DECREF(map);
			return 0;
		}
	}

	return 1;
}

int construct_handle_start_array(JsonSlicer* self) {
	PyObject* array = PyList_New(0);
	if (!array) {
		return 0;
	}

	if (!push_constructing_object(self, array)) {
		Py_DECREF(array);
		return 0;
	}
	return 1;
}

int construct_handle_end_array(JsonSlicer* self) {
	PyObject* array = pop_constructing_object(self);

	if (pyobjlist_empty(&self->constructing)) {
		if (!finish_complete_object(self, array)) {
			Py_DECREF(array);
			return 0;
		}
	}

	return 1;
}
