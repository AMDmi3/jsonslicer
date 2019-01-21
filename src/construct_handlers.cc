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
bool add_to_parent(JsonSlicer* self, PyObjPtr value) {
	PyObjPtr container = self->constructing.back();

	if (PyDict_Check(container.get())) {
		if (!PyBytes_Check(self->last_map_key.get()) && !PyUnicode_Check(self->last_map_key.get())) {
			PyErr_SetString(PyExc_RuntimeError, "No map key available");
			return false;
		}

		// adds references
		if (PyDict_SetItem(container.get(), self->last_map_key.get(), value.get()) != 0) {
			return false;
		}
	} else if (PyList_Check(container.get())) {
		// adds reference
		return PyList_Append(container.get(), value.get()) == 0;
	} else {
		PyErr_SetString(PyExc_RuntimeError, "Unexpected container type on the stack");
		return false;
	}

	return true;
}
