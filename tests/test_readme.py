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
    "friends": [
        {"name": "John", "age": 31},
        {"name": "Ivan", "age": 26}
    ],
    "colleagues": {
        "manager": {"name": "Jack", "age": 33},
        "subordinate": {"name": "Lucy", "age": 21}
    }
}
"""


class TestReadmeExamples(unittest.TestCase):
    def test_readme_examples(self):
        self.assertEqual(
            run_js(JSON, ('friends', 1, 'age')),
            [26]
        )

        self.assertEqual(
            run_js(JSON, ('colleagues', 'manager', 'name')),
            ['Jack']
        )

        self.assertEqual(
            run_js(JSON, ('friends', None)),
            [
                {'name': 'John', 'age': 31},
                {'name': 'Ivan', 'age': 26},
            ]
        )

        self.assertEqual(
            run_js(JSON, (None, None)),
            [
                {'name': 'John', 'age': 31},
                {'name': 'Ivan', 'age': 26},
                {'name': 'Jack', 'age': 33},
                {'name': 'Lucy', 'age': 21},
            ]
        )

        self.assertEqual(
            run_js(JSON, ('colleagues', None), path_mode='map_keys'),
            [
                ('manager', {'name': 'Jack', 'age': 33}),
                ('subordinate', {'name': 'Lucy', 'age': 21}),
            ]
        )

        self.assertEqual(
            run_js(JSON, (None, None), path_mode='full'),
            [
                ('friends', 0, {'name': 'John', 'age': 31}),
                ('friends', 1, {'name': 'Ivan', 'age': 26}),
                ('colleagues', 'manager', {'name': 'Jack', 'age': 33}),
                ('colleagues', 'subordinate', {'name': 'Lucy', 'age': 21}),
            ]
        )

        self.assertEqual(
            sum(run_js(JSON, (None, None, 'age'))),
            111
        )


if __name__ == '__main__':
    unittest.main()
