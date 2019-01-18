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

#include <Python.h>

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

// scalars
int handle_null(void* ctx) {
	JsonSlicer* self = (JsonSlicer*)ctx;
	if (self->mode == MODE_CONSTRUCTING) {
		return construct_handle_null(self);
	} else {
		return seek_handle_null(self);
	}
}

int handle_boolean(void* ctx, int val) {
	JsonSlicer* self = (JsonSlicer*)ctx;
	if (self->mode == MODE_CONSTRUCTING) {
		return construct_handle_boolean(self, val);
	} else {
		return seek_handle_boolean(self, val);
	}
}

int handle_integer(void* ctx, long long val) {
	JsonSlicer* self = (JsonSlicer*)ctx;
	if (self->mode == MODE_CONSTRUCTING) {
		return construct_handle_integer(self, val);
	} else {
		return seek_handle_integer(self, val);
	}
}

int handle_double(void* ctx, double val) {
	JsonSlicer* self = (JsonSlicer*)ctx;
	if (self->mode == MODE_CONSTRUCTING) {
		return construct_handle_double(self, val);
	} else {
		return seek_handle_double(self, val);
	}
}

int handle_string(void* ctx, const unsigned char* str, size_t len) {
	JsonSlicer* self = (JsonSlicer*)ctx;
	if (self->mode == MODE_CONSTRUCTING) {
		return construct_handle_string(self, (const char*)str, len);
	} else {
		return seek_handle_string(self, (const char*)str, len);
	}
}

// map key
int handle_map_key(void* ctx, const unsigned char* str, size_t len) {
	JsonSlicer* self = (JsonSlicer*)ctx;
	if (self->mode == MODE_CONSTRUCTING) {
		return construct_handle_map_key(self, (const char*)str, len);
	} else {
		return seek_handle_map_key(self, (const char*)str, len);
	}
}

// containers
int handle_start_map(void* ctx) {
	JsonSlicer* self = (JsonSlicer*)ctx;
	if (self->mode == MODE_CONSTRUCTING) {
		return construct_handle_start_map(self);
	} else {
		return seek_handle_start_map(self);
	}
}

int handle_end_map(void* ctx) {
	JsonSlicer* self = (JsonSlicer*)ctx;
	if (self->mode == MODE_CONSTRUCTING) {
		return construct_handle_end_map(self);
	} else {
		return seek_handle_end_map(self);
	}
}

int handle_start_array(void* ctx) {
	JsonSlicer* self = (JsonSlicer*)ctx;
	if (self->mode == MODE_CONSTRUCTING) {
		return construct_handle_start_array(self);
	} else {
		return seek_handle_start_array(self);
	}
}

int handle_end_array(void* ctx) {
	JsonSlicer* self = (JsonSlicer*)ctx;
	if (self->mode == MODE_CONSTRUCTING) {
		return construct_handle_end_array(self);
	} else {
		return seek_handle_end_array(self);
	}
}
