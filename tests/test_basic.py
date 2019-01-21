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

import io
import unittest

from jsonslicer import JsonSlicer


class TestJsonSlicerBasic(unittest.TestCase):
    def test_constructs(self):
        self.assertIsNotNone(JsonSlicer(io.StringIO('0'), ()))

    def test_allows_next(self):
        self.assertIsNotNone(next(JsonSlicer(io.StringIO('0'), ())))

    def test_allows_list(self):
        self.assertIsNotNone(list(JsonSlicer(io.StringIO('0'), ())))

    def test_accepts_bytes(self):
        self.assertIsNotNone(JsonSlicer(io.BytesIO(b'0'), ()))
        self.assertIsNotNone(next(JsonSlicer(io.BytesIO(b'0'), ())))

    def test_accepts_unicode(self):
        self.assertIsNotNone(JsonSlicer(io.StringIO('0'), ()))
        self.assertIsNotNone(next(JsonSlicer(io.BytesIO(b'0'), ())))


if __name__ == '__main__':
    unittest.main()
