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

static PyObject* JsonSlicer_new(PyTypeObject *type, PyObject *args, PyObject *kwds) {
	JsonSlicer* self;

	self = (JsonSlicer*)type->tp_alloc(type, 0);
	if (self != NULL) {
		self->io = NULL;
		self->yajl = NULL;

		self->last_map_key = NULL;
		self->mode = MODE_SEEKING;

		self->pattern_head = NULL;
		self->pattern_tail = NULL;

		self->complete_head = NULL;
		self->complete_tail = NULL;

		self->path_head = NULL;
		self->path_tail = NULL;

		self->constructing_head = NULL;
		self->constructing_tail = NULL;
	}

	return (PyObject*)self;
}

static void JsonSlicer_deinit(JsonSlicer* self) {
	pyobjlist_clear(&self->constructing_head, &self->constructing_tail);
	pyobjlist_clear(&self->path_head, &self->path_tail);
	pyobjlist_clear(&self->complete_head, &self->complete_tail);
	Py_CLEAR(self->last_map_key);
	if (self->yajl)
		yajl_free(self->yajl);
	Py_CLEAR(self->io);
}

static void JsonSlicer_dealloc(JsonSlicer* self) {
	JsonSlicer_deinit(self);

	Py_TYPE(self)->tp_free((PyObject*)self);
}

static int JsonSlicer_init(JsonSlicer* self, PyObject* args, PyObject* kwds) {
	JsonSlicer_deinit(self);

	// parse args
	PyObject* io = NULL;
	PyObject* pattern = NULL;

	if (!PyArg_ParseTuple(args, "OO", &io, &pattern))
        return -1;

	// (re)init
	if (io) {
		Py_INCREF(io);
		self->io = io;
	}

	if (pattern) {
		for (Py_ssize_t i = 0; i < PySequence_Size(pattern); i++) {
			PyObject* item = PySequence_GetItem(pattern, i);
			Py_INCREF(item);
			Py_INCREF(item);
			Py_INCREF(item);
			Py_INCREF(item);
			if (!pyobjlist_push_back(&self->pattern_head, &self->pattern_tail, item)) {
				// XXX
				return -1;
			}
		}
	}

	self->yajl = yajl_alloc(&yajl_handlers, NULL, (void*)self);
	if (self->yajl == NULL)
		return -1;
	self->mode = MODE_SEEKING;

	return 0;
}

static JsonSlicer* JsonSlicer_iter(JsonSlicer* self) {
	Py_INCREF(self);
	return self;
}

static PyObject* JsonSlicer_iternext(JsonSlicer* self) {
	// return complete objects from previous runs, if any
	PyObject* complete = pyobjlist_pop_front(&self->complete_head, &self->complete_tail);
	if (complete) {
		return complete;
	}

	PyObject* read = PyDict_GetItemString(self->io, "read");
	int read_size = 1024;
	int eof = 0;

	do {
		// read chunk of data from IO
		PyObject* buffer = PyObject_CallMethod(self->io, "read", "i", read_size);

		// handle i/o errors
		if (!buffer) {
			return NULL;
		}
		if (!PyBytes_Check(buffer)) {
			PyErr_Format(PyExc_RuntimeError, "Unexpected read result type %s, expected bytes", buffer->ob_type->tp_name);
			Py_XDECREF(buffer);
			return NULL;
		}

		// advance or finalize parser
		yajl_status status;
		if (PyBytes_GET_SIZE(buffer) == 0) {
			eof = 1;
			status = yajl_complete_parse(self->yajl);
		} else {
			status = yajl_parse(self->yajl, (const unsigned char*)PyBytes_AS_STRING(buffer), PyBytes_GET_SIZE(buffer));
		}

		// handle parser errors
		if (status != yajl_status_ok) {
			unsigned char* error = yajl_get_error(self->yajl, 1, (const unsigned char*)PyBytes_AS_STRING(buffer), PyBytes_GET_SIZE(buffer));
			if (status == yajl_status_error) {
				PyErr_Format(PyExc_RuntimeError, "YAJL error: %s", error);
			} // else it's interrupted parsing and PyErr is already set
			yajl_free_error(self->yajl, error);
			Py_XDECREF(buffer);
			return NULL;
		}

		// free buffer
		Py_XDECREF(buffer);

		// return complete object, if any
		PyObject* complete = pyobjlist_pop_front(&self->complete_head, &self->complete_tail);
		if (complete) {
			return complete;
		}
	} while (!eof);

	return NULL;
}

PyTypeObject JsonSlicerType = {
	PyVarObject_HEAD_INIT(NULL, 0)
	"jsonslicer.JsonSlicer",   /* tp_name */
	sizeof(JsonSlicer),        /* tp_basicsize */
	0,                         /* tp_itemsize */
	(destructor)JsonSlicer_dealloc, /* tp_dealloc */
	0,                         /* tp_print */
	0,                         /* tp_getattr */
	0,                         /* tp_setattr */
	0,                         /* tp_reserved */
	0,                         /* tp_repr */
	0,                         /* tp_as_number */
	0,                         /* tp_as_sequence */
	0,                         /* tp_as_mapping */
	0,                         /* tp_hash  */
	0,                         /* tp_call */
	0,                         /* tp_str */
	0,                         /* tp_getattro */
	0,                         /* tp_setattro */
	0,                         /* tp_as_buffer */
	Py_TPFLAGS_DEFAULT,        /* tp_flags */
	"JsonSlicer objects",      /* tp_doc */
	0,                         /* tp_traverse */
	0,                         /* tp_clear */
	0,                         /* tp_richcompare */
	0,                         /* tp_weaklistoffset */
	(getiterfunc)JsonSlicer_iter, /* tp_iter */
    (iternextfunc)JsonSlicer_iternext, /* tp_iternext */
	0,                         /* tp_methods */
    0,                         /* tp_members */
    0,                         /* tp_getset */
    0,                         /* tp_base */
    0,                         /* tp_dict */
    0,                         /* tp_descr_get */
    0,                         /* tp_descr_set */
    0,                         /* tp_dictoffset */
    (initproc)JsonSlicer_init, /* tp_init */
    0,                         /* tp_alloc */
    JsonSlicer_new,            /* tp_new */
};
