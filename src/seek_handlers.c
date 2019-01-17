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

#include "seek_handlers.h"

#include "construct_handlers.h"

#include "pyobjlist.h"

#include <Python.h>

// helpers
static int path_matches_pattern(PyObject* path, PyObject* pattern) {
	return pattern == Py_None || PyObject_RichCompareBool(path, pattern, Py_EQ);
}

static int check_pattern(JsonSlicer* self) {
	return pyobjlist_match(&self->path, &self->pattern, &path_matches_pattern);
}

int handle_path_change(JsonSlicer* self) {
	if (self->path.back && PyLong_Check(self->path.back->obj)) {
		PyObject* old_index = self->path.back->obj;
		long long value = PyLong_AsLongLong(old_index);
		if (value == -1 && PyErr_Occurred()) {
			return 0;
		}

		PyObject* new_index = PyLong_FromLongLong(value + 1);
		if (!new_index) {
			return 0;
		}
		self->path.back->obj = new_index;
		Py_DECREF(old_index);
	}

	return 1;
}

int finish_complete_object(JsonSlicer* self, PyObject* obj) {
	// regardless of result, we've finished parsing an object
	self->mode = MODE_SEEKING;

	// construct tuple with prepended path
	PyObject* tuple = pyobjlist_as_tuple_prefix(&self->path, obj);
	if (tuple == NULL) {
		return 0;
	}

	// save in list of complete objects
	if (!pyobjlist_push_back(&self->complete, tuple)) {
		Py_DECREF(tuple);
		return 0;
	}

	return handle_path_change(self);
}

// scalars
int seek_handle_null(JsonSlicer* self) {
	if (check_pattern(self)) {
		self->mode = MODE_CONSTRUCTING;
		return construct_handle_null(self);
	}
	return handle_path_change(self);
}

int seek_handle_boolean(JsonSlicer* self, int val) {
	if (check_pattern(self)) {
		self->mode = MODE_CONSTRUCTING;
		return construct_handle_boolean(self, val);
	}
	return handle_path_change(self);
}

int seek_handle_integer(JsonSlicer* self, long long val) {
	if (check_pattern(self)) {
		self->mode = MODE_CONSTRUCTING;
		return construct_handle_integer(self, val);
	}
	return handle_path_change(self);
}

int seek_handle_double(JsonSlicer* self, double val) {
	if (check_pattern(self)) {
		self->mode = MODE_CONSTRUCTING;
		return construct_handle_double(self, val);
	}
	return handle_path_change(self);
}

int seek_handle_string(JsonSlicer* self, const char* str, size_t len) {
	if (check_pattern(self)) {
		self->mode = MODE_CONSTRUCTING;
		return construct_handle_string(self, str, len);
	}
	return handle_path_change(self);
}

// map key
int seek_handle_map_key(JsonSlicer* self, const char* str, size_t len) {
	assert(self->path.back && self->path.back->obj);

	Py_DECREF(self->path.back->obj);

	self->path.back->obj = PyBytes_FromStringAndSize(str, len);
	if (self->path.back->obj == NULL) {
		return 0;
	}
	return 1;
}

// containers
int seek_handle_start_map(JsonSlicer* self) {
	if (check_pattern(self)) {
		self->mode = MODE_CONSTRUCTING;
		return construct_handle_start_map(self);
	} else {
		Py_INCREF(Py_None);
		return pyobjlist_push_back(&self->path, Py_None);
	}
}

int seek_handle_end_map(JsonSlicer* self) {
	PyObject* popped = pyobjlist_pop_back(&self->path);
	assert(popped);
	Py_DECREF(popped);
	return handle_path_change(self);
}

int seek_handle_start_array(JsonSlicer* self) {
	if (check_pattern(self)) {
		self->mode = MODE_CONSTRUCTING;
		return construct_handle_start_array(self);
	} else {
		PyObject* index = PyLong_FromLong(0);
		if (index == NULL) {
			return 0;
		}
		return pyobjlist_push_back(&self->path, index);
	}
}

int seek_handle_end_array(JsonSlicer* self) {
	PyObject* popped = pyobjlist_pop_back(&self->path);
	assert(popped);
	Py_DECREF(popped);
	return handle_path_change(self);
}
