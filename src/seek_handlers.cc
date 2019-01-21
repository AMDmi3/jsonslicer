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
bool check_pattern(JsonSlicer* self) {
	return self->path.match(self->pattern, [](const PyObjPtr& path, const PyObjPtr& pattern) {
		return pattern.get() == Py_None || PyObject_RichCompareBool(path.get(), pattern.get(), Py_EQ);
	});
}

void update_path(JsonSlicer* self) {
	if (!self->path.empty() && PyMutIndex_Check(self->path.back().get())) {
		PyMutIndex_Increment(self->path.back().get());
	}
}

bool finish_complete_object(JsonSlicer* self, PyObjPtr obj) {
	// regardless of result, we've finished parsing an object
	self->state = JsonSlicer::State::SEEKING;

	// construct tuple with prepended path
	PyObjPtr output = generate_output_object(self, obj);
	if (!output.valid()) {
		return false;
	}

	// save in list of complete objects
	if (!self->complete.push_back(output)) {
		return false;
	}

	update_path(self);
	return true;
}
