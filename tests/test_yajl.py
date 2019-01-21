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

from .common import runJS


class TestJsonSlicerYajlFlags(unittest.TestCase):
    def test_yajl_allow_comments_off(self):
        with self.assertRaises(RuntimeError):
            runJS('1 // comment')

    def test_yajl_allow_comments_off(self):
        self.assertEqual(
            runJS('1 // comment', yajl_allow_comments=True),
            [1]
        )

    def test_yajl_dont_validate_strings_on(self):
        with self.assertRaises(RuntimeError):
            runJS(b'"\xff"', binary=True)

    def test_yajl_dont_validate_strings_off(self):
        self.assertEqual(
            runJS(b'"\xff"', binary=True, yajl_dont_validate_strings=True),
            [b'\xff']
        )

    def test_yajl_allow_trailing_garbage_on(self):
        with self.assertRaises(RuntimeError):
            runJS('{}{}')

    def test_yajl_allow_trailing_garbage_off(self):
        self.assertEqual(
            runJS('{}{}', (), yajl_allow_trailing_garbage=True),
            [{}]
        )

    def test_yajl_allow_multiple_values_on(self):
        with self.assertRaises(RuntimeError):
            runJS('{}{}')

    def test_yajl_allow_multiple_values_off(self):
        self.assertEqual(
            runJS('{}{}', yajl_allow_multiple_values=True),
            [{}, {}]
        )

    def test_yajl_allow_partial_values_on(self):
        with self.assertRaises(RuntimeError):
            runJS('[1', (None,))

    def test_yajl_allow_partial_values_off(self):
        self.assertEqual(
            runJS('[1', (None,), yajl_allow_partial_values=True),
            [1]
        )


if __name__ == '__main__':
    unittest.main()
