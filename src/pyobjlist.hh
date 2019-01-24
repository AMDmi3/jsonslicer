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

#ifndef JSONSLICER_PYOBJLIST_HH
#define JSONSLICER_PYOBJLIST_HH

#include <Python.h>

#include "pyobjptr.hh"

struct PyObjList {
private:
	struct Node {
		PyObjPtr obj;
		Node* next = nullptr;
		Node* prev = nullptr;
	};

public:
	class iterator {
	private:
		Node* node_;

	public:
		explicit iterator(Node* node) : node_(node) {
		}

		PyObjPtr operator*() {
			return node_->obj;
		}

		bool operator!=(iterator& other) {
			return node_ != other.node_;
		}

		iterator& operator++() {
			node_ = node_->next;
			return *this;
		}

		iterator operator++(int) {
			iterator tmp(node_);
			node_ = node_->next;
			return tmp;
		}
	};

private:
	Node* front_;
	Node* back_;

public:
	PyObjList();
	~PyObjList();

	PyObjList(const PyObjList&) = delete;
	PyObjList& operator=(const PyObjList&) = delete;
	PyObjList(PyObjList&&) = delete;
	PyObjList& operator=(PyObjList&&) = delete;

	void clear();

	iterator begin();
	iterator end();

	size_t size() const;
	bool empty() const;

	bool push_front(PyObjPtr obj);
	bool push_back(PyObjPtr obj);

	PyObjPtr pop_front();
	PyObjPtr pop_back();

	PyObjPtr& back() const;

	void swap(PyObjList& other);

	template <class T>
	bool match(const PyObjList& other, T&& equals) {
		Node* lnode = front_;
		Node* rnode = other.front_;

		for (; lnode != nullptr && rnode != nullptr; lnode = lnode->next, rnode = rnode->next) {
			if (!equals(lnode->obj, rnode->obj)) {
				return false;
			}
		}

		return lnode == nullptr && rnode == nullptr;
	}

	template <class T>
	void foreach(T&& func) {
		for (Node* node = front_; node != nullptr; node = node->next) {
			func(node->obj);
		}
	}
};

#endif
