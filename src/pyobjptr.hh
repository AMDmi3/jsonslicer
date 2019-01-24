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

#ifndef JSONSLICER_PYOBJPTR_HH
#define JSONSLICER_PYOBJPTR_HH

#include <Python.h>

class PyObjPtr {
private:
	PyObject* obj_;

private:
	explicit PyObjPtr(PyObject* obj) noexcept: obj_(obj) {
	}

public:
	PyObjPtr() noexcept: obj_(nullptr) {
	}

	~PyObjPtr() noexcept {
		if (obj_ != nullptr) {
			assert(Py_REFCNT(obj_));
			Py_DECREF(obj_);
		}
	}

	PyObjPtr(const PyObjPtr& other) noexcept: obj_(other.obj_) {
		if (other.obj_ != nullptr) {
			Py_INCREF(obj_);
		}
	}

	PyObjPtr& operator=(const PyObjPtr& other) noexcept {
		PyObject* tmp = obj_;
		if (other.obj_ != nullptr) {
			Py_INCREF(other.obj_);
		}
		obj_ = other.obj_;
		Py_XDECREF(tmp);
		return *this;
	}

	PyObjPtr(PyObjPtr&& other) noexcept: obj_(other.obj_) {
		other.obj_ = nullptr;
	}

	PyObjPtr& operator=(PyObjPtr&& other) noexcept {
		PyObject* tmp = obj_;
		obj_ = other.obj_;
		other.obj_ = nullptr;
		Py_XDECREF(tmp);
		return *this;
	}

	PyObject* release() noexcept {
		assert(obj_);
		PyObject* tmp = obj_;
		obj_ = nullptr;
		return tmp;
	}

	PyObject* get() const noexcept {
		assert(obj_);
		return obj_;
	}

	PyObject* getref() const noexcept {
		assert(obj_);
		Py_INCREF(obj_);
		return obj_;
	}

	bool valid() const noexcept {
		return obj_ != nullptr;
	}

	operator bool() const noexcept {
		return obj_ != nullptr;
	}

public:
	static PyObjPtr Borrow(PyObject* obj) noexcept {
		if (obj != nullptr) {
			Py_INCREF(obj);
		}
		return PyObjPtr(obj);
	}

	static PyObjPtr Take(PyObject* obj) noexcept {
		return PyObjPtr(obj);
	}
};

#endif
