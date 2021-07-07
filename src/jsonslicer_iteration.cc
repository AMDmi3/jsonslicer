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

#include "encoding.hh"

#include <Python.h>
#include <yajl/yajl_parse.h>

JsonSlicer* JsonSlicer_iter(JsonSlicer* self) {
	Py_INCREF(self);
	return self;
}

PyObject* JsonSlicer_iternext(JsonSlicer* self) {
	// return complete objects from previous runs, if any
	if (!self->complete.empty()) {
		return self->complete.pop_front().release();
	}

	bool eof = false;

	do {
		// read chunk of data from IO
		PyObjPtr buffer = PyObjPtr::Take(PyObject_CallMethod(self->io.get(), "read", "n", self->read_size));

		// handle i/o errors
		if (!buffer) {
			return nullptr;
		}
		if (PyUnicode_Check(buffer.get())) {
			PyObjPtr encoded = encode(buffer, self->input_encoding, self->input_errors);
			if (!encoded) {
				return nullptr;
			}
			buffer = encoded;
		}
		if (!PyBytes_Check(buffer.get())) {
			PyErr_Format(PyExc_RuntimeError, "Unexpected read result type %s, expected bytes", buffer.get()->ob_type->tp_name);
			return nullptr;
		}

		// advance or finalize parser
		yajl_status status;
		if (PyBytes_GET_SIZE(buffer.get()) == 0) {
			eof = true;
			status = yajl_complete_parse(self->yajl);
		} else {
			status = yajl_parse(self->yajl, (const unsigned char*)PyBytes_AS_STRING(buffer.get()), PyBytes_GET_SIZE(buffer.get()));
		}

		// handle parser errors
		if (status != yajl_status_ok) {
			if (status == yajl_status_error) {
				unsigned char* error = yajl_get_error(self->yajl, self->yajl_verbose_errors, (const unsigned char*)PyBytes_AS_STRING(buffer.get()), PyBytes_GET_SIZE(buffer.get()));
				PyErr_Format(PyExc_RuntimeError, "YAJL error: %s", error);
				yajl_free_error(self->yajl, error);
			} // else it's interrupted parsing and PyErr is already set
			return nullptr;
		}

		// return complete object, if any
		if (!self->complete.empty()) {
			return self->complete.pop_front().release();
		}
	} while (!eof);

	return nullptr;
}
