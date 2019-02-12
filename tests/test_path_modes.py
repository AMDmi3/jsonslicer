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


JSON = """
{
    "a":{
        "b":1
    },
    "c":[
        2
    ]
}
"""


class TestPathModes(unittest.TestCase):
    def test_path_mode_ignore(self):
        self.assertEqual(
            run_js(JSON, (None, None), path_mode='ignore'),
            [
                1,
                2
            ]
        )

    def test_path_mode_map_keys(self):
        self.assertEqual(
            run_js(JSON, (None, None), path_mode='map_keys'),
            [
                ('b', 1),
                2
            ]
        )

    def test_path_mode_full(self):
        self.assertEqual(
            run_js(JSON, (None, None), path_mode='full'),
            [
                ('a', 'b', 1),
                ('c', 0, 2)
            ]
        )


if __name__ == '__main__':
    unittest.main()
