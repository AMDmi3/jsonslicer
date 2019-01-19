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

int PyMutIndex_Check(PyObject* p) {
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
		return NULL;
	}

	int equals = 0;
	if (PyLong_Check(other)) {
		size_t other_value = PyLong_AsSize_t(other);

		if (other_value == (size_t)-1 && PyErr_Occurred()) {
			return NULL;
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
	PyVarObject_HEAD_INIT(NULL, 0)
	"jsonslicer.PyMutIndex",   /* tp_name */
	sizeof(PyMutIndex),        /* tp_basicsize */
	0,                         /* tp_itemsize */
	0,                         /* tp_dealloc */
	0,                         /* tp_print */
	0,                         /* tp_getattr */
	0,                         /* tp_setattr */
	0,                         /* tp_reserved */
	0,                         /* tp_repr */
	0,                         /* tp_as_number */
	0,                         /* tp_as_sequence */
	0,                         /* tp_as_mapping */
	0,                         /* tp_hash  */
	0,                         /* tp_call */
	0,                         /* tp_str */
	0,                         /* tp_getattro */
	0,                         /* tp_setattro */
	0,                         /* tp_as_buffer */
	Py_TPFLAGS_DEFAULT,        /* tp_flags */
	"PyMutIndex objects",      /* tp_doc */
	0,                         /* tp_traverse */
	0,                         /* tp_clear */
	PyMutIndex_RichCompare,    /* tp_richcompare */
	0,                         /* tp_weaklistoffset */
	0,                         /* tp_iter */
	0,                         /* tp_iternext */
	0,                         /* tp_methods */
	0,                         /* tp_members */
	0,                         /* tp_getset */
	0,                         /* tp_base */
	0,                         /* tp_dict */
	0,                         /* tp_descr_get */
	0,                         /* tp_descr_set */
	0,                         /* tp_dictoffset */
	0,                         /* tp_init */
	0,                         /* tp_alloc */
	0,                         /* tp_new */
};
