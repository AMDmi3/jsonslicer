# Copyright (c) 2019 Dmitry Marakasov <amdmi3@amdmi3.ru>
#
# Permission is hereby granted, free of charge, to any person obtaining a copy
# of this software and associated documentation files (the "Software"), to deal
# in the Software without restriction, including without limitation the rights
# to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
# copies of the Software, and to permit persons to whom the Software is
# furnished to do so, subject to the following conditions:
#
# The above copyright notice and this permission notice shall be included in
# all copies or substantial portions of the Software.
#
# THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
# IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
# FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
# AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
# LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
# OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
# THE SOFTWARE.

import unittest

from .common import run_js


class TestJsonSlicerTypesFromRoot(unittest.TestCase):
    def test_parses_int_from_root(self):
        self.assertEqual(run_js('1000000', ()), [1000000])

    def test_parses_sint_from_root(self):
        self.assertEqual(run_js('-1000000', ()), [-1000000])

    def test_parses_double_from_root(self):
        self.assertEqual(run_js('0.33', ()), [0.33])

    def test_parses_string_from_root(self):
        self.assertEqual(run_js('"test"', ()), ['test'])

    def test_parses_none_from_root(self):
        self.assertEqual(run_js('null', ()), [None])

    def test_parses_true_from_root(self):
        self.assertEqual(run_js('true', ()), [True])

    def test_parses_false_from_root(self):
        self.assertEqual(run_js('false', ()), [False])

    def test_parses_array_from_root(self):
        self.assertEqual(run_js('[]', ()), [[]])

    def test_parses_dict_from_root(self):
        self.assertEqual(run_js('{}', ()), [{}])


class TestJsonSlicerTypesFromContainer(unittest.TestCase):
    def test_parses_int_from_container(self):
        self.assertEqual(run_js('[1000000]', (None,)), [1000000])

    def test_parses_sint_from_container(self):
        self.assertEqual(run_js('[-1000000]', (None,)), [-1000000])

    def test_parses_double_from_container(self):
        self.assertEqual(run_js('[0.33]', (None,)), [0.33])

    def test_parses_string_from_container(self):
        self.assertEqual(run_js('["test"]', (None,)), ['test'])

    def test_parses_none_from_container(self):
        self.assertEqual(run_js('[null]', (None,)), [None])

    def test_parses_true_from_container(self):
        self.assertEqual(run_js('[true]', (None,)), [True])

    def test_parses_false_from_container(self):
        self.assertEqual(run_js('[false]', (None,)), [False])

    def test_parses_array_from_container(self):
        self.assertEqual(run_js('[[]]', (None,)), [[]])

    def test_parses_dict_from_container(self):
        self.assertEqual(run_js('[{}]', (None,)), [{}])


class TestJsonSlicerTypesFromNestedContainer(unittest.TestCase):
    def test_parses_int_from_nested(self):
        self.assertEqual(run_js('[[1000000]]', (None,)), [[1000000]])

    def test_parses_sint_from_nested(self):
        self.assertEqual(run_js('[[-1000000]]', (None,)), [[-1000000]])

    def test_parses_double_from_nested(self):
        self.assertEqual(run_js('[[0.33]]', (None,)), [[0.33]])

    def test_parses_string_from_nested(self):
        self.assertEqual(run_js('[["test"]]', (None,)), [['test']])

    def test_parses_none_from_nested(self):
        self.assertEqual(run_js('[[null]]', (None,)), [[None]])

    def test_parses_true_from_nested(self):
        self.assertEqual(run_js('[[true]]', (None,)), [[True]])

    def test_parses_false_from_nested(self):
        self.assertEqual(run_js('[[false]]', (None,)), [[False]])

    def test_parses_array_from_nested(self):
        self.assertEqual(run_js('[[[]]]', (None,)), [[[]]])

    def test_parses_dict_from_nested(self):
        self.assertEqual(run_js('[[{}]]', (None,)), [[{}]])


if __name__ == '__main__':
    unittest.main()
