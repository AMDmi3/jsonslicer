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

#ifndef JSONSLICER_HANDLERS_HH
#define JSONSLICER_HANDLERS_HH

#include <yajl/yajl_parse.h>

extern const yajl_callbacks yajl_handlers;

int handle_null(void* ctx);
int handle_boolean(void* ctx, int val);
int handle_integer(void* ctx, long long val);
int handle_double(void* ctx, double val);
int handle_string(void* ctx, const unsigned char* str, size_t len);
int handle_start_map(void* ctx);
int handle_map_key(void* ctx, const unsigned char* str, size_t len);
int handle_end_map(void* ctx);
int handle_start_array(void* ctx);
int handle_end_array(void* ctx);

#endif
