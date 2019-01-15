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
import json
import os
import random
import sys
import unittest

if os.environ.get('TRACEMALLOC'):
    import gc
    import tracemalloc

from jsonslicer import JsonSlicer


def deep_encode(obj, encoding):
    if isinstance(obj, str):
        return obj.encode(encoding)
    elif isinstance(obj, list):
        return [deep_encode(item, encoding) for item in obj]
    elif isinstance(obj, tuple):
        return tuple((deep_encode(item, encoding) for item in obj))
    elif isinstance(obj, dict):
        return {deep_encode(key, encoding): deep_encode(value, encoding) for key, value in obj.items()}
    else:
        return obj


def deep_decode(obj, encoding):
    if isinstance(obj, bytes):
        return obj.decode(encoding)
    elif isinstance(obj, list):
        return [deep_decode(item, encoding) for item in obj]
    elif isinstance(obj, tuple):
        return tuple((deep_decode(item, encoding) for item in obj))
    elif isinstance(obj, dict):
        return {deep_decode(key, encoding): deep_decode(value, encoding) for key, value in obj.items()}
    else:
        return obj


def rand_bytes():
    return str(int(random.random() * 100)).encode('utf-8')


class TestJsonSlicer(unittest.TestCase):
    def assertNoLeaks(self, func):
        tracemalloc.start(25)

        # run function once so unrelated objects get allocated
        func()

        gc.collect()
        snapshot1 = tracemalloc.take_snapshot()

        # run function again to monitor new allocations
        func()

        gc.collect()
        snapshot2 = tracemalloc.take_snapshot()
        tracemalloc.stop()

        filters = (
            tracemalloc.Filter(False, tracemalloc.__file__),
        )
        snapshot1 = snapshot1.filter_traces(filters)
        snapshot2 = snapshot2.filter_traces(filters)

        leak_message = ''
        for stat in snapshot2.compare_to(snapshot1, 'lineno'):
            if stat.size_diff != 0:
                leak_message += 'tracemalloc: possible leak: {} bytes, {} allocations\n'.format(stat.size_diff, stat.count_diff)
                for line in stat.traceback.format():
                    leak_message += line + '\n'

        if leak_message:
            self.fail('Memory leaks detected\n' + leak_message)


    def run_checks(self, data, cases):
        json_bytes = json.dumps(data).encode('utf-8')

        for path, expected in cases.items():
            pseudofile = io.BytesIO(json_bytes)
            slicer = JsonSlicer(pseudofile, deep_encode(path, 'utf-8'))
            results = deep_decode(list(slicer), 'utf-8')

            self.assertEqual(results, expected)

    @unittest.skipIf(not os.environ.get('TRACEMALLOC'), 'TRACEMALLOC not set')
    def test_leaks_construct(self):
        def wrapper():
            JsonSlicer(io.BytesIO(b'0'), ())

        self.assertNoLeaks(wrapper)

    @unittest.skipIf(not os.environ.get('TRACEMALLOC'), 'TRACEMALLOC not set')
    def test_leaks_construct_with_path(self):
        def wrapper():
            JsonSlicer(io.BytesIO(b'0'), (rand_bytes(), rand_bytes()))

        self.assertNoLeaks(wrapper)

    @unittest.skipIf(not os.environ.get('TRACEMALLOC'), 'TRACEMALLOC not set')
    def test_leaks_reinit(self):
        def wrapper():
            js = JsonSlicer(io.BytesIO(b'0'), (rand_bytes(), rand_bytes()))
            js.__init__(io.BytesIO(b'0'), (rand_bytes(), rand_bytes()))

        self.assertNoLeaks(wrapper)

    @unittest.skipIf(not os.environ.get('TRACEMALLOC'), 'TRACEMALLOC not set')
    def test_leaks_iterate(self):
        def wrapper():
            js = JsonSlicer(io.BytesIO(b'[0,1,2]'), (None,))

        self.assertNoLeaks(wrapper)


    def test_just_next(self):
        self.assertIsNotNone(next(JsonSlicer(io.BytesIO(b'0'), ())))

    def test_root_elems(self):
        self.assertEqual(*next(JsonSlicer(io.BytesIO(b'0'), ())), 0)
        self.assertEqual(*next(JsonSlicer(io.BytesIO(b'1000000'), ())), 1000000)
        self.assertEqual(*next(JsonSlicer(io.BytesIO(b'-1000000'), ())), -1000000)
        self.assertEqual(*next(JsonSlicer(io.BytesIO(b'0.3'), ())), 0.3)
        self.assertEqual(*next(JsonSlicer(io.BytesIO(b'"string"'), ())), b'string')
        self.assertEqual(*next(JsonSlicer(io.BytesIO(b'null'), ())), None)
        self.assertEqual(*next(JsonSlicer(io.BytesIO(b'true'), ())), True)
        self.assertEqual(*next(JsonSlicer(io.BytesIO(b'false'), ())), False)
        self.assertEqual(*next(JsonSlicer(io.BytesIO(b'[]'), ())), [])
        self.assertEqual(*next(JsonSlicer(io.BytesIO(b'{}'), ())), {})

    def test_types(self):
        data = [
            0,
            10000000,
            -10000000,
            0.3,
            'string',
            None,
            True,
            False,
            [],
            {},
        ]

        cases = {(idx,): [(idx, value)] for idx, value in enumerate(data)}

        self.run_checks(data, cases)

    def test_exact_paths(self):
        data = {
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
        }

        cases = {
            (): [(data,)],
            ('a',): [('a', data['a'])],
            ('b',): [('b', data['b'])],
            ('a', 'a'): [('a', 'a', data['a']['a'])],
            ('a', 'b'): [('a', 'b', data['a']['b'])],
            ('b', 'a'): [('b', 'a', data['b']['a'])],
            ('b', 'b'): [('b', 'b', data['b']['b'])],
            ('a', 'a', 0): [('a', 'a', 0, data['a']['a'][0])],
            ('a', 'a', 1): [('a', 'a', 1, data['a']['a'][1])],
            ('a', 'b', 0): [('a', 'b', 0, data['a']['b'][0])],
            ('a', 'b', 1): [('a', 'b', 1, data['a']['b'][1])],
            ('b', 'a', 0): [('b', 'a', 0, data['b']['a'][0])],
            ('b', 'a', 1): [('b', 'a', 1, data['b']['a'][1])],
            ('b', 'b', 0): [('b', 'b', 0, data['b']['b'][0])],
            ('b', 'b', 1): [('b', 'b', 1, data['b']['b'][1])],
        }

        self.run_checks(data, cases)

    def test_pattern_paths(self):
        data = {
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

        cases = {
            (None,): [
                ('a', data['a']),
                ('b', data['b']),
                ('c', data['c']),
            ],

            ('a', None): [
                ('a', 'a', data['a']['a']),
                ('a', 'b', data['a']['b']),
            ],
            ('b', None): [
                ('b', 'a', data['b']['a']),
                ('b', 'b', data['b']['b']),
            ],
            (None, 'a'): [
                ('a', 'a', data['a']['a']),
                ('b', 'a', data['b']['a']),
            ],
            (None, 'b'): [
                ('a', 'b', data['a']['b']),
                ('b', 'b', data['b']['b']),
            ],
            (None, None): [
                ('a', 'a', data['a']['a']),
                ('a', 'b', data['a']['b']),
                ('b', 'a', data['b']['a']),
                ('b', 'b', data['b']['b']),
            ],

            ('a', None, None): [
                ('a', 'a', 0, data['a']['a'][0]),
                ('a', 'a', 1, data['a']['a'][1]),
                ('a', 'b', 0, data['a']['b'][0]),
                ('a', 'b', 1, data['a']['b'][1]),
            ],
            (None, 'a', None): [
                ('a', 'a', 0, data['a']['a'][0]),
                ('a', 'a', 1, data['a']['a'][1]),
                ('b', 'a', 0, data['b']['a'][0]),
                ('b', 'a', 1, data['b']['a'][1]),
            ],
            (None, None, 0): [
                ('a', 'a', 0, data['a']['a'][0]),
                ('a', 'b', 0, data['a']['b'][0]),
                ('b', 'a', 0, data['b']['a'][0]),
                ('b', 'b', 0, data['b']['b'][0]),
            ],
            (None, None, None): [
                ('a', 'a', 0, data['a']['a'][0]),
                ('a', 'a', 1, data['a']['a'][1]),
                ('a', 'b', 0, data['a']['b'][0]),
                ('a', 'b', 1, data['a']['b'][1]),
                ('b', 'a', 0, data['b']['a'][0]),
                ('b', 'a', 1, data['b']['a'][1]),
                ('b', 'b', 0, data['b']['b'][0]),
                ('b', 'b', 1, data['b']['b'][1]),
            ],
        }

        self.run_checks(data, cases)

    def test_examples(self):
        data = b"""
        {
            "people": [
                {"name":"John","age":31},
                {"name": "Ivan", "age": 26},
                {"name": "Angela", "age": 33}
            ]
        }"""

        for *_, person in JsonSlicer(io.BytesIO(data), (b'people', None)):
            pass

        self.assertEqual(person[b'name'], b'Angela')

        *_, person in JsonSlicer(io.BytesIO(data), (b'people', 2))
        self.assertEqual(person[b'name'], b'Angela')

        max_age = max((age for *_, age in JsonSlicer(io.BytesIO(data), (b'people', None, b'age'))))
        self.assertEqual(max_age, 33)


if __name__ == '__main__':
    unittest.main()
