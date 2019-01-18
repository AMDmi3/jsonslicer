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

#include "seek_handlers.hh"
#include "construct_handlers.hh"
#include "pymutindex.hh"

#include <Python.h>

template<class T> bool generic_handle_scalar(JsonSlicer* self, T&& make_scalar) {
	if (self->mode == MODE_SEEKING) {
		if (check_pattern(self)) {
			self->mode = MODE_CONSTRUCTING;
			// falls through to MODE_CONSTRUCTING block below
		} else {
			update_path(self);
			return true;
		}
	}
	if (self->mode == MODE_CONSTRUCTING) {
		PyObject* scalar = make_scalar();
		if (scalar == NULL) {
			return false;
		}

		bool res;
		if (pyobjlist_empty(&self->constructing)) {
			res = finish_complete_object(self, scalar);
		} else {
			res = add_to_parent(self, scalar);
		}
		if (!res) {
			Py_DECREF(scalar);
		}

		return res;
	}
	return true;
}

template<class T, class U>
bool generic_start_container(JsonSlicer* self, T&& make_container, U&& make_key) {
	if (self->mode == MODE_SEEKING) {
	    if (check_pattern(self)) {
			self->mode = MODE_CONSTRUCTING;
			// falls through to MODE_CONSTRUCTING block below
		} else {
			PyObject* key = make_key();
			if (key == NULL) {
				return false;
			}
			return pyobjlist_push_back(&self->path, key);
		}
	}
	if (self->mode == MODE_CONSTRUCTING) {
		PyObject* container = make_container();
		if (container == NULL) {
			return false;
		}

		if (!push_constructing_object(self, container)) {
			Py_DECREF(container);
			return false;
		}
	}
	return true;
}

bool generic_end_container(JsonSlicer* self) {
	if (self->mode == MODE_SEEKING) {
		PyObject* container = pyobjlist_pop_back(&self->path);
		assert(container);
		Py_DECREF(container);
		update_path(self);
	}
	if (self->mode == MODE_CONSTRUCTING) {
		PyObject* container = pop_constructing_object(self);

		if (pyobjlist_empty(&self->constructing)) {
			if (!finish_complete_object(self, container)) {
				Py_DECREF(container);
				return false;
			}
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
	NULL,
	handle_string,
	handle_start_map,
	handle_map_key,
	handle_end_map,
	handle_start_array,
	handle_end_array
};

int handle_null(void* ctx) {
	return generic_handle_scalar((JsonSlicer*)ctx, [](){
		Py_RETURN_NONE;
	});
}

int handle_boolean(void* ctx, int val) {
	return generic_handle_scalar((JsonSlicer*)ctx, [val](){
		if (val) {
			Py_RETURN_TRUE;
		} else {
			Py_RETURN_FALSE;
		}
	});
}

int handle_integer(void* ctx, long long val) {
	return generic_handle_scalar((JsonSlicer*)ctx, [val](){
		return PyLong_FromLongLong(val);
	});
}

int handle_double(void* ctx, double val) {
	return generic_handle_scalar((JsonSlicer*)ctx, [val](){
		return PyFloat_FromDouble(val);
	});
}

int handle_string(void* ctx, const unsigned char* str, size_t len) {
	return generic_handle_scalar((JsonSlicer*)ctx, [str, len](){
		return PyBytes_FromStringAndSize(reinterpret_cast<const char*>(str), len);
	});
}

// map key
int handle_map_key(void* ctx, const unsigned char* str, size_t len) {
	JsonSlicer* self = (JsonSlicer*)ctx;

	PyObject* new_map_key = PyBytes_FromStringAndSize(reinterpret_cast<const char*>(str), len);
	if (new_map_key == NULL) {
		return false;
	}
	if (self->mode == MODE_CONSTRUCTING) {
		PyObject* old_map_key = self->last_map_key;
		self->last_map_key = new_map_key;
		Py_XDECREF(old_map_key);
	} else {
		assert(self->path.back && self->path.back->obj);

		PyObject* old_path_tail = self->path.back->obj;
		self->path.back->obj = new_map_key;
		Py_DECREF(old_path_tail);
	}
	return true;
}

// containers
int handle_start_map(void* ctx) {
	return generic_start_container(
		(JsonSlicer*)ctx,
		[]{ return PyDict_New(); },
		[]{ Py_RETURN_NONE; }
	);
}

int handle_end_map(void* ctx) {
	return generic_end_container((JsonSlicer*)ctx);
}

int handle_start_array(void* ctx) {
	return generic_start_container(
		(JsonSlicer*)ctx,
		[]{ return PyList_New(0); },
		[]{ return PyMutIndex_New(); }
	);
}

int handle_end_array(void* ctx) {
	return generic_end_container((JsonSlicer*)ctx);
}
