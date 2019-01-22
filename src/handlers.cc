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

#include "handlers.hh"

#include "output_formatting.hh"
#include "encoding.hh"
#include "seek_handlers.hh"
#include "construct_handlers.hh"
#include "pymutindex.hh"

#include <Python.h>

template<class T> bool generic_handle_scalar(JsonSlicer* self, T&& make_scalar) {
	if (self->state == JsonSlicer::State::SEEKING) {
		if (check_pattern(self)) {
			self->state = JsonSlicer::State::CONSTRUCTING;
			// falls through to JsonSlicer::State::CONSTRUCTING block below
		} else {
			update_path(self);
			return true;
		}
	}
	if (self->state == JsonSlicer::State::CONSTRUCTING) {
		PyObjPtr scalar = make_scalar();
		if (scalar) {
			scalar = decode(scalar, self->output_encoding, self->output_errors);
		}
		if (!scalar) {
			return false;
		}

		if (self->constructing.empty()) {
			return finish_complete_object(self, scalar);
		} else {
			return add_to_parent(self, scalar);
		}
	}
	return true;
}

template<class T, class U>
bool generic_start_container(JsonSlicer* self, T&& make_container, U&& make_key) {
	if (self->state == JsonSlicer::State::SEEKING) {
	    if (check_pattern(self)) {
			self->state = JsonSlicer::State::CONSTRUCTING;
			// falls through to JsonSlicer::State::CONSTRUCTING block below
		} else {
			PyObjPtr key = make_key();
			if (!key.valid()) {
				return false;
			}
			return self->path.push_back(key);
		}
	}
	if (self->state == JsonSlicer::State::CONSTRUCTING) {
		PyObjPtr container = make_container();
		if (!container.valid()) {
			return false;
		}

		if (!self->constructing.empty()) {
			if (!add_to_parent(self, container)) {
				return false;
			}
		}

		return self->constructing.push_back(container);
	}
	return true;
}

bool generic_end_container(JsonSlicer* self) {
	if (self->state == JsonSlicer::State::SEEKING) {
		PyObjPtr container = self->path.pop_back();
		assert(container);
		update_path(self);
	}
	if (self->state == JsonSlicer::State::CONSTRUCTING) {
		PyObjPtr container = self->constructing.pop_back();

		if (self->constructing.empty()) {
			return finish_complete_object(self, container);
		}
	}
	return true;
}

// scalars
const yajl_callbacks yajl_handlers = {
	handle_null,
	handle_boolean,
	handle_integer,
	handle_double,
	nullptr,
	handle_string,
	handle_start_map,
	handle_map_key,
	handle_end_map,
	handle_start_array,
	handle_end_array
};

int handle_null(void* ctx) {
	return generic_handle_scalar((JsonSlicer*)ctx, [](){
		return PyObjPtr::Borrow(Py_None);
	});
}

int handle_boolean(void* ctx, int val) {
	return generic_handle_scalar((JsonSlicer*)ctx, [val](){
		return PyObjPtr::Borrow(val ? Py_True : Py_False);
	});
}

int handle_integer(void* ctx, long long val) {
	return generic_handle_scalar((JsonSlicer*)ctx, [val](){
		return PyObjPtr::Take(PyLong_FromLongLong(val));
	});
}

int handle_double(void* ctx, double val) {
	return generic_handle_scalar((JsonSlicer*)ctx, [val](){
		return PyObjPtr::Take(PyFloat_FromDouble(val));
	});
}

int handle_string(void* ctx, const unsigned char* str, size_t len) {
	return generic_handle_scalar((JsonSlicer*)ctx, [str, len](){
		return PyObjPtr::Take(PyBytes_FromStringAndSize(reinterpret_cast<const char*>(str), len));
	});
}

// map key
int handle_map_key(void* ctx, const unsigned char* str, size_t len) {
	JsonSlicer* self = (JsonSlicer*)ctx;

	PyObjPtr key = PyObjPtr::Take(PyBytes_FromStringAndSize(reinterpret_cast<const char*>(str), len));
#ifdef USE_BYTES_INTERNALLY
	if (key && self->state == JsonSlicer::State::CONSTRUCTING) {
		key = decode(key, self->output_encoding, self->output_errors);
	}
#else // use output encoding internally
	if (key) {
		key = decode(key, self->output_encoding, self->output_errors);
	}
#endif
	if (!key.valid()) {
		return false;
	}

	if (self->state == JsonSlicer::State::CONSTRUCTING) {
		self->last_map_key = key;
	} else {
		self->path.back() = key;
	}
	return true;
}

// containers
int handle_start_map(void* ctx) {
	return generic_start_container(
		(JsonSlicer*)ctx,
		[]{ return PyObjPtr::Take(PyDict_New()); },
		[]{ return PyObjPtr::Borrow(Py_None); }
	);
}

int handle_end_map(void* ctx) {
	return generic_end_container((JsonSlicer*)ctx);
}

int handle_start_array(void* ctx) {
	return generic_start_container(
		(JsonSlicer*)ctx,
		[]{ return PyObjPtr::Take(PyList_New(0)); },
		[]{ return PyObjPtr::Take(PyMutIndex_New()); }
	);
}

int handle_end_array(void* ctx) {
	return generic_end_container((JsonSlicer*)ctx);
}
