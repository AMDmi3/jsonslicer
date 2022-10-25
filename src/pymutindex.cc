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

#include "pymutindex.hh"

bool PyMutIndex_Check(PyObject* p) {
	return Py_TYPE(p) == &PyMutIndex_type;
}

PyObject* PyMutIndex_New() {
	PyMutIndex* obj = (PyMutIndex*)PyMutIndex_type.tp_alloc(&PyMutIndex_type, 0);
	obj->value = 0;
	return (PyObject*)obj;
}

void PyMutIndex_Increment(PyObject* index) {
	((PyMutIndex*)index)->value++;
}

PyObject* PyMutIndex_AsPyLong(PyObject* index) {
	return PyLong_FromSize_t(((PyMutIndex*)index)->value);
}

static PyObject* PyMutIndex_RichCompare(PyObject* index, PyObject* other, int opid) {
	if (opid != Py_EQ && opid != Py_NE) {
		PyErr_SetString(PyExc_TypeError, "PyMytIndex only supports EQ/NE comparison with integers");
		return nullptr;
	}

	bool equals = false;
	if (PyLong_Check(other)) {
		size_t other_value = PyLong_AsSize_t(other);

		if (other_value == (size_t)-1 && PyErr_Occurred()) {
			return nullptr;
		}

		equals = ((PyMutIndex*)index)->value == other_value;
	}

	if (equals == (opid == Py_EQ)) {
		Py_RETURN_TRUE;
	} else {
		Py_RETURN_FALSE;
	}
}

PyTypeObject PyMutIndex_type = {
	PyVarObject_HEAD_INIT(nullptr, 0)
	"jsonslicer.PyMutIndex",   // tp_name
	sizeof(PyMutIndex),        // tp_basicsize
	0,                         // tp_itemsize
	nullptr,                   // tp_dealloc
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
	"PyMutIndex objects",      // tp_doc
	nullptr,                   // tp_traverse
	nullptr,                   // tp_clear
	PyMutIndex_RichCompare,    // tp_richcompare
	0,                         // tp_weaklistoffset
	nullptr,                   // tp_iter
	nullptr,                   // tp_iternext
	nullptr,                   // tp_methods
	nullptr,                   // tp_members
	nullptr,                   // tp_getset
	nullptr,                   // tp_base
	nullptr,                   // tp_dict
	nullptr,                   // tp_descr_get
	nullptr,                   // tp_descr_set
	0,                         // tp_dictoffset
	nullptr,                   // tp_init
	nullptr,                   // tp_alloc
	nullptr,                   // tp_new
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
