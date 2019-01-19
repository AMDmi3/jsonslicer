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

#include "jsonslicer.hh"

#include <Python.h>
#include <yajl/yajl_parse.h>

JsonSlicer* JsonSlicer_iter(JsonSlicer* self) {
	Py_INCREF(self);
	return self;
}

PyObject* JsonSlicer_iternext(JsonSlicer* self) {
	// return complete objects from previous runs, if any
	PyObject* complete = pyobjlist_pop_front(&self->complete);
	if (complete) {
		return complete;
	}

	bool eof = false;

	do {
		// read chunk of data from IO
		PyObject* buffer = PyObject_CallMethod(self->io, "read", "n", self->read_size);

		// handle i/o errors
		if (!buffer) {
			return nullptr;
		}
		if (!PyBytes_Check(buffer)) {
			PyErr_Format(PyExc_RuntimeError, "Unexpected read result type %s, expected bytes", buffer->ob_type->tp_name);
			Py_XDECREF(buffer);
			return nullptr;
		}

		// advance or finalize parser
		yajl_status status;
		if (PyBytes_GET_SIZE(buffer) == 0) {
			eof = true;
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
			return nullptr;
		}

		// free buffer
		Py_XDECREF(buffer);

		// return complete object, if any
		PyObject* complete = pyobjlist_pop_front(&self->complete);
		if (complete) {
			return complete;
		}
	} while (!eof);

	return nullptr;
}
