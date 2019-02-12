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

import json
import unittest

from .common import run_js


DATA = {
    'a': {
        'a': [
            100,
            101,
        ],
        'b': [
            102,
            103,
        ],
    },
    'b': {
        'a': [
            104,
            105,
        ],
        'b': [
            106,
            107,
        ],
    },
    'c': 108,  # mixup with shorter path
}


JSON = json.dumps(DATA, sort_keys=True)


class TestJsonSlicerPaths(unittest.TestCase):
    def test_exact_paths(self):
        cases = {
            (): [(DATA,)],
            ('a',): [('a', DATA['a'])],
            ('b',): [('b', DATA['b'])],
            ('a', 'a'): [('a', 'a', DATA['a']['a'])],
            ('a', 'b'): [('a', 'b', DATA['a']['b'])],
            ('b', 'a'): [('b', 'a', DATA['b']['a'])],
            ('b', 'b'): [('b', 'b', DATA['b']['b'])],
            ('a', 'a', 0): [('a', 'a', 0, DATA['a']['a'][0])],
            ('a', 'a', 1): [('a', 'a', 1, DATA['a']['a'][1])],
            ('a', 'b', 0): [('a', 'b', 0, DATA['a']['b'][0])],
            ('a', 'b', 1): [('a', 'b', 1, DATA['a']['b'][1])],
            ('b', 'a', 0): [('b', 'a', 0, DATA['b']['a'][0])],
            ('b', 'a', 1): [('b', 'a', 1, DATA['b']['a'][1])],
            ('b', 'b', 0): [('b', 'b', 0, DATA['b']['b'][0])],
            ('b', 'b', 1): [('b', 'b', 1, DATA['b']['b'][1])],
        }

        for path, result in cases.items():
            self.assertEqual(
                run_js(JSON, path, path_mode='full'),
                result
            )

    def test_pattern_paths(self):
        cases = {
            (None,): [
                ('a', DATA['a']),
                ('b', DATA['b']),
                ('c', DATA['c']),
            ],

            ('a', None): [
                ('a', 'a', DATA['a']['a']),
                ('a', 'b', DATA['a']['b']),
            ],
            ('b', None): [
                ('b', 'a', DATA['b']['a']),
                ('b', 'b', DATA['b']['b']),
            ],
            (None, 'a'): [
                ('a', 'a', DATA['a']['a']),
                ('b', 'a', DATA['b']['a']),
            ],
            (None, 'b'): [
                ('a', 'b', DATA['a']['b']),
                ('b', 'b', DATA['b']['b']),
            ],
            (None, None): [
                ('a', 'a', DATA['a']['a']),
                ('a', 'b', DATA['a']['b']),
                ('b', 'a', DATA['b']['a']),
                ('b', 'b', DATA['b']['b']),
            ],

            ('a', None, None): [
                ('a', 'a', 0, DATA['a']['a'][0]),
                ('a', 'a', 1, DATA['a']['a'][1]),
                ('a', 'b', 0, DATA['a']['b'][0]),
                ('a', 'b', 1, DATA['a']['b'][1]),
            ],
            (None, 'a', None): [
                ('a', 'a', 0, DATA['a']['a'][0]),
                ('a', 'a', 1, DATA['a']['a'][1]),
                ('b', 'a', 0, DATA['b']['a'][0]),
                ('b', 'a', 1, DATA['b']['a'][1]),
            ],
            (None, None, 0): [
                ('a', 'a', 0, DATA['a']['a'][0]),
                ('a', 'b', 0, DATA['a']['b'][0]),
                ('b', 'a', 0, DATA['b']['a'][0]),
                ('b', 'b', 0, DATA['b']['b'][0]),
            ],
            (None, None, None): [
                ('a', 'a', 0, DATA['a']['a'][0]),
                ('a', 'a', 1, DATA['a']['a'][1]),
                ('a', 'b', 0, DATA['a']['b'][0]),
                ('a', 'b', 1, DATA['a']['b'][1]),
                ('b', 'a', 0, DATA['b']['a'][0]),
                ('b', 'a', 1, DATA['b']['a'][1]),
                ('b', 'b', 0, DATA['b']['b'][0]),
                ('b', 'b', 1, DATA['b']['b'][1]),
            ],
        }

        for path, result in cases.items():
            self.assertEqual(
                run_js(JSON, path, path_mode='full'),
                result
            )


if __name__ == '__main__':
    unittest.main()
