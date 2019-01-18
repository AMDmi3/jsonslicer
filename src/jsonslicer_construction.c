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

#include "jsonslicer.h"

#include "handlers.h"

#include <Python.h>
#include <yajl/yajl_parse.h>

PyObject* JsonSlicer_new(PyTypeObject *type, PyObject *args, PyObject *kwds) {
	JsonSlicer* self = (JsonSlicer*)type->tp_alloc(type, 0);
	if (self != NULL) {
		self->io = NULL;
		self->read_size = 1024;  // XXX: bump somewhat for production use
		self->path_mode = PATHMODE_IGNORE;

		self->yajl = NULL;

		self->last_map_key = NULL;
		self->mode = MODE_SEEKING;

		pyobjlist_init(&self->pattern);
		pyobjlist_init(&self->path);
		pyobjlist_init(&self->constructing);
		pyobjlist_init(&self->complete);
	}
	return (PyObject*)self;
}

void JsonSlicer_dealloc(JsonSlicer* self) {
	pyobjlist_clear(&self->complete);
	pyobjlist_clear(&self->constructing);
	pyobjlist_clear(&self->path);
	pyobjlist_clear(&self->pattern);

	Py_CLEAR(self->last_map_key);

	if (self->yajl != NULL) {
		yajl_handle tmp = self->yajl;
		self->yajl = NULL;
		yajl_free(tmp);
	}
	Py_CLEAR(self->io);

	Py_TYPE(self)->tp_free((PyObject*)self);
}

int JsonSlicer_init(JsonSlicer* self, PyObject* args, PyObject* kwargs) {
	// parse args
	PyObject* io = NULL;
	PyObject* pattern = NULL;
	Py_ssize_t read_size = self->read_size;
	int path_mode = self->path_mode;

	static char *keywords[] = {"file", "path_prefix", "read_size", "path_mode", NULL};

	const char* path_mode_arg = NULL;
	if (!PyArg_ParseTupleAndKeywords(args, kwargs, "OO|$ns", keywords, &io, &pattern, &read_size, &path_mode_arg)) {
		return -1;
	}

	if (path_mode_arg) {
		if (strcmp(path_mode_arg, "ignore") == 0) {
			path_mode = PATHMODE_IGNORE;
		} else if (strcmp(path_mode_arg, "map_keys") == 0) {
			path_mode = PATHMODE_MAP_KEYS;
		} else if (strcmp(path_mode_arg, "full") == 0) {
			path_mode = PATHMODE_FULL;
		} else {
			PyErr_SetString(PyExc_ValueError, "Bad value for path_mode argument");
			return -1;
		}
	}

	assert(io != NULL);
	assert(pattern != NULL);

	// prepare all new data members
	PyObjList new_pattern;
	pyobjlist_init(&new_pattern);

	for (Py_ssize_t i = 0; i < PySequence_Size(pattern); i++) {
		PyObject* item = PySequence_GetItem(pattern, i);
		if (item == NULL) {
			pyobjlist_clear(&new_pattern);
			return -1;
		}
		if (!pyobjlist_push_back(&new_pattern, item)) {
			pyobjlist_clear(&new_pattern);
			Py_DECREF(item);
			return -1;
		}
	}

	yajl_handle new_yajl = yajl_alloc(&yajl_handlers, NULL, (void*)self);
	if (new_yajl == NULL) {
		pyobjlist_clear(&new_pattern);
		return -1;
	}

	Py_INCREF(io);

	// swap initialized members with new ones, clearing the rest
	pyobjlist_clear(&self->complete);

	pyobjlist_clear(&self->constructing);

	pyobjlist_clear(&self->path);

	{
		PyObjList tmp = self->pattern;
		self->pattern = new_pattern;
		pyobjlist_clear(&tmp);
	}

	self->mode = MODE_SEEKING;

	Py_CLEAR(self->last_map_key);

	{
		yajl_handle tmp = self->yajl;
		self->yajl = new_yajl;
		if (tmp != NULL) {
			yajl_free(tmp);
		}
	}

	self->path_mode = path_mode;
	self->read_size = read_size;

	{
		PyObject* tmp = self->io;
		self->io = io;
		Py_XDECREF(tmp);
	}

	return 0;
}
