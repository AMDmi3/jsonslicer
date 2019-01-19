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

#include "seek_handlers.hh"

#include "construct_handlers.hh"

#include "output_formatting.hh"

#include "pyobjlist.hh"
#include "pymutindex.hh"

#include <Python.h>

// helpers
static int path_matches_pattern(PyObject* path, PyObject* pattern) {
	return pattern == Py_None || PyObject_RichCompareBool(path, pattern, Py_EQ);
}

int check_pattern(JsonSlicer* self) {
	return pyobjlist_match(&self->path, &self->pattern, &path_matches_pattern);
}

void update_path(JsonSlicer* self) {
	if (self->path.back && PyMutIndex_Check(self->path.back->obj)) {
		PyMutIndex_Increment(self->path.back->obj);
	}
}

int finish_complete_object(JsonSlicer* self, PyObject* obj) {
	// regardless of result, we've finished parsing an object
	self->state = JsonSlicer::State::SEEKING;

	// construct tuple with prepended path
	PyObject* output = generate_output_object(self, obj);
	if (output == nullptr) {
		return 0;
	}

	// save in list of complete objects
	if (!pyobjlist_push_back(&self->complete, output)) {
		Py_DECREF(output);
		return 0;
	}

	update_path(self);
	return 1;
}
