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

#include "output_formatting.hh"

#include "encoding.hh"
#include "pyobjlist.hh"
#include "pymutindex.hh"

PyObjPtr generate_output_object(JsonSlicer* self, PyObjPtr obj) {
	if (self->path_mode == JsonSlicer::PathMode::IGNORE) {
		return obj;
	} else if (self->path_mode == JsonSlicer::PathMode::MAP_KEYS) {
		if (self->path.empty() || (!PyBytes_Check(self->path.back().get()) && !PyUnicode_Check(self->path.back().get()))) {
			return obj;
		} else {
			PyObjPtr tuple = PyObjPtr::Take(PyTuple_New(2));
			if (!tuple.valid()) {
				return {};
			}
			PyObjPtr pathel = decode(self->path.back(), self->output_encoding, self->output_errors);
			if (!pathel) {
				return {};
			}
			PyTuple_SET_ITEM(tuple.get(), 0, pathel.getref());
			PyTuple_SET_ITEM(tuple.get(), 1, obj.getref());
			return tuple;
		}
	} else if (self->path_mode == JsonSlicer::PathMode::FULL) {
		PyObjPtr tuple = PyObjPtr::Take(PyTuple_New(self->path.size() + 1));
		if (!tuple.valid()) {
			return {};
		}

		size_t tuple_idx = 0;
		for (auto pathel: self->path) {
			if (PyMutIndex_Check(pathel.get())) {
				PyObjPtr index = PyObjPtr::Take(PyMutIndex_AsPyLong(pathel.get()));
				if (!index.valid()) {
					return {};
				}
				PyTuple_SET_ITEM(tuple.get(), tuple_idx++, index.getref());
			} else {
				pathel = decode(pathel, self->output_encoding, self->output_errors);
				if (!pathel) {
					return {};
				}
				PyTuple_SET_ITEM(tuple.get(), tuple_idx++, pathel.getref());
			}
		}

		PyTuple_SET_ITEM(tuple.get(), tuple_idx, obj.getref());

		return tuple;
	} else {
		PyErr_SetString(PyExc_RuntimeError, "Unexpected path mode");
		return {};
	}
}
