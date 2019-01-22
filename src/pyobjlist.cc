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

#include "pyobjlist.hh"
#include "pymutindex.hh"

#include <stdlib.h>

#include <algorithm>
#include <cassert>

PyObjList::PyObjList(): front_(nullptr), back_(nullptr) {
}

PyObjList::~PyObjList() {
	clear();
}

void PyObjList::clear() {
	Node* cur = back_;

	front_ = nullptr;
	back_ = nullptr;

	while (cur != nullptr) {
		Node* tmp = cur;
		cur = cur->prev;
		delete tmp;
	}
}

PyObjList::iterator PyObjList::begin() {
	return PyObjList::iterator(front_);
}

PyObjList::iterator PyObjList::end() {
	return PyObjList::iterator(nullptr);;
}

size_t PyObjList::size() const {
	size_t count = 0;

	for (Node* node = front_; node; node = node->next) {
		count++;
	}

	return count;
}

bool PyObjList::empty() const {
	return front_ == nullptr;
}

bool PyObjList::push_front(PyObjPtr obj) {
	Node* node = new(std::nothrow) Node;
	if (node == nullptr)
		return false;

	node->obj = obj;
	node->prev = nullptr;
	node->next = front_;

	if (node->next) {
		node->next->prev = node;
	} else {
		back_ = node;
	}

	front_ = node;

	return true;
}

bool PyObjList::push_back(PyObjPtr obj) {
	Node* node = new(std::nothrow) Node;
	if (node == nullptr)
		return false;

	node->obj = obj;
	node->prev = back_;
	node->next = nullptr;

	if (node->prev) {
		node->prev->next = node;
	} else {
		front_ = node;
	}

	back_ = node;

	return true;
}

PyObjPtr PyObjList::pop_front() {
	Node* node = front_;
	assert(node);

	if (node->next) {
		node->next->prev = nullptr;
	} else {
		back_ = nullptr;
	}

	front_ = node->next;

	PyObjPtr result = node->obj;
	delete node;

	return result;
}

PyObjPtr PyObjList::pop_back() {
	Node* node = back_;
	assert(node);

	if (node->prev) {
		back_->prev->next = nullptr;
	} else {
		front_ = nullptr;
	}

	back_ = node->prev;

	PyObjPtr result = node->obj;
	delete node;

	return result;
}

PyObjPtr& PyObjList::back() const {
	assert(back_);
	return back_->obj;
}

void PyObjList::swap(PyObjList& other) {
	std::swap(front_, other.front_);
	std::swap(back_, other.back_);
}
