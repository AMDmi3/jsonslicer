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
#include "pymutindex.hh"

#include <Python.h>

static struct PyModuleDef jsonslicer_module_def = {
	PyModuleDef_HEAD_INIT,
	.m_name = "jsonslicer",
	.m_doc = "jsonslicer module",
	.m_size = -1,
};

PyMODINIT_FUNC PyInit_jsonslicer(void) {
	if (PyType_Ready(&JsonSlicerType) < 0)
		return NULL;
	if (PyType_Ready(&PyMutIndex_type) < 0)
		return NULL;

	PyObject* m = PyModule_Create(&jsonslicer_module_def);
	if (m == NULL)
		return NULL;

	Py_INCREF(&JsonSlicerType);
	PyModule_AddObject(m, "JsonSlicer", (PyObject*)&JsonSlicerType);

	return m;
}
