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

PyTypeObject JsonSlicerType = {
	PyVarObject_HEAD_INIT(nullptr, 0)
	"jsonslicer.JsonSlicer",   // tp_name
	sizeof(JsonSlicer),        // tp_basicsize
	0,                         // tp_itemsize
	(destructor)JsonSlicer_dealloc, // tp_dealloc
#if PY_VERSION_HEX >= 0x03080000
	0,                         // tp_vectorcall_offset
#else
	nullptr,                   // tp_print
#endif
	nullptr,                   // tp_getattr
	nullptr,                   // tp_setattr
	nullptr,                   // tp_reserved
	nullptr,                   // tp_repr
	nullptr,                   // tp_as_number
	nullptr,                   // tp_as_sequence
	nullptr,                   // tp_as_mapping
	nullptr,                   // tp_hash
	nullptr,                   // tp_call
	nullptr,                   // tp_str
	nullptr,                   // tp_getattro
	nullptr,                   // tp_setattro
	nullptr,                   // tp_as_buffer
	Py_TPFLAGS_DEFAULT,        // tp_flags
	"JsonSlicer objects",      // tp_doc
	nullptr,                   // tp_traverse
	nullptr,                   // tp_clear
	nullptr,                   // tp_richcompare
	0,                         // tp_weaklistoffset
	(getiterfunc)JsonSlicer_iter, // tp_iter
	(iternextfunc)JsonSlicer_iternext, // tp_iternext
	nullptr,                   // tp_methods
	nullptr,                   // tp_members
	nullptr,                   // tp_getset
	nullptr,                   // tp_base
	nullptr,                   // tp_dict
	nullptr,                   // tp_descr_get
	nullptr,                   // tp_descr_set
	0,                         // tp_dictoffset
	(initproc)JsonSlicer_init, // tp_init
	nullptr,                   // tp_alloc
	JsonSlicer_new,            // tp_new
	nullptr,                   // tp_free
	nullptr,                   // tp_is_gc
	nullptr,                   // tp_bases
	nullptr,                   // tp_mro
	nullptr,                   // tp_cache
	nullptr,                   // tp_subclasses
	nullptr,                   // tp_weaklist
	nullptr,                   // tp_del
	0,                         // tp_version_tag
	nullptr,                   // tp_finalize
#if PY_VERSION_HEX >= 0x03080000
	nullptr,                   // tp_vectorcall
#if PY_VERSION_HEX < 0x03090000
	nullptr,                   // tp_print
#endif
#endif
};
