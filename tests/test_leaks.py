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
import os
import unittest

from jsonslicer import JsonSlicer

if os.environ.get('TRACEMALLOC'):
    import gc
    import tracemalloc


gen_index = 0


def gen_bytes():
    global gen_index
    gen_index += 1
    return ('gen_' + str(gen_index)).encode('utf-8')


@unittest.skipIf(not os.environ.get('TRACEMALLOC'), 'TRACEMALLOC not set')
class TestJsonSlicer(unittest.TestCase):
    def assertNoLeaks(self, func):  # noqa: N802
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

    def test_leaks_construct(self):
        def wrapper():
            JsonSlicer(io.BytesIO(b'0'), ())

        self.assertNoLeaks(wrapper)

    def test_leaks_construct_with_path(self):
        def wrapper():
            JsonSlicer(io.BytesIO(b'0'), (gen_bytes(), gen_bytes()))

        self.assertNoLeaks(wrapper)

    def test_leaks_reinit(self):
        def wrapper():
            js = JsonSlicer(io.BytesIO(b'0'), (gen_bytes(), gen_bytes()))
            js.__init__(io.BytesIO(b'0'), (gen_bytes(), gen_bytes()))

        self.assertNoLeaks(wrapper)

    def test_leaks_iterate(self):
        def wrapper():
            js = JsonSlicer(io.BytesIO(b'[0,1,2]'), (None,))
            next(js)

        self.assertNoLeaks(wrapper)


if __name__ == '__main__':
    unittest.main()
