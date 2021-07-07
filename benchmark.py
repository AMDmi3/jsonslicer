#!/usr/bin/env python3

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

import argparse
import io
import json
import time

import ijson.backends.python as ijson_python
import ijson.backends.yajl2 as ijson_yajl2
import ijson.backends.yajl2_c as ijson_yajl2_c
import ijson.backends.yajl2_cffi as ijson_yajl2_cffi

from jsonslicer import JsonSlicer

from tabulate import tabulate


class TestCase:
    def __init__(self, name, type_, iters, results):
        self.results = results
        self.name = name
        self.type = type_
        self.iters = iters

    def __enter__(self):
        self.start_time = time.monotonic()

    def __exit__(self, exc_type, exc_val, exc_tb):
        elapsed = time.monotonic() - self.start_time

        self.results.append((
            self.name,
            self.type,
            '{:.1f}K'.format(self.iters / elapsed / 1000) if not exc_type else '*failed*'
        ))

        return True


if __name__ == '__main__':
    parser = argparse.ArgumentParser(formatter_class=argparse.ArgumentDefaultsHelpFormatter)
    parser.add_argument('-n', '--json-size', type=int, default=100000, help='number of objects to generate')
    args = parser.parse_args()

    jsondata = '{"level1":{"level2":[' + ','.join(('{{"id":{}}}'.format(i) for i in range(args.json_size))) + ']}}'

    results = []

    with TestCase('json.loads()', 'str', args.json_size, results):
        for n, item in enumerate(json.loads(jsondata)['level1']['level2']):
            assert(item['id'] == n)

    with TestCase('json.load(StringIO())', 'str', args.json_size, results):
        gen = io.StringIO(jsondata)
        for n, item in enumerate(json.load(gen)['level1']['level2']):
            assert(item['id'] == n)

    with TestCase('**JsonSlicer (no paths, binary input, binary output)**', 'bytes', args.json_size, results):
        gen = io.BytesIO(jsondata.encode('utf-8'))
        parser = JsonSlicer(gen, (b'level1', b'level2', None), binary=True)
        for n, item in enumerate(parser):
            assert(item[b'id'] == n)

    with TestCase('**JsonSlicer (no paths, unicode input, binary output)**', 'bytes', args.json_size, results):
        gen = io.StringIO(jsondata)
        parser = JsonSlicer(gen, (b'level1', b'level2', None), binary=True)
        for n, item in enumerate(parser):
            assert(item[b'id'] == n)

    with TestCase('**JsonSlicer (no paths, binary input, unicode output)**', 'str', args.json_size, results):
        gen = io.BytesIO(jsondata.encode('utf-8'))
        parser = JsonSlicer(gen, ('level1', 'level2', None))
        for n, item in enumerate(parser):
            assert(item['id'] == n)

    with TestCase('**JsonSlicer (no paths, unicode input, unicode output)**', 'str', args.json_size, results):
        gen = io.StringIO(jsondata)
        parser = JsonSlicer(gen, ('level1', 'level2', None))
        for n, item in enumerate(parser):
            assert(item['id'] == n)

    with TestCase('**JsonSlicer (full paths, binary output)**', 'bytes', args.json_size, results):
        gen = io.BytesIO(jsondata.encode('utf-8'))
        parser = JsonSlicer(gen, (b'level1', b'level2', None), path_mode='full', binary=True)
        for n, (*path, item) in enumerate(parser):
            assert(item[b'id'] == n)

    with TestCase('**JsonSlicer (full paths, unicode output)**', 'str', args.json_size, results):
        gen = io.StringIO(jsondata)
        parser = JsonSlicer(gen, ('level1', 'level2', None), path_mode='full')
        for n, (*path, item) in enumerate(parser):
            assert(item['id'] == n)

    with TestCase('ijson.yajl2_c', 'bytes', args.json_size, results):
        gen = io.BytesIO(jsondata.encode('utf-8'))
        parser = ijson_yajl2_c.items(gen, b'level1.level2.item')
        for n, item in enumerate(parser):
            assert(item['id'] == n)

    with TestCase('ijson.yajl2_cffi', 'bytes', args.json_size, results):
        gen = io.BytesIO(jsondata.encode('utf-8'))
        parser = ijson_yajl2_cffi.items(gen, b'level1.level2.item')
        for n, item in enumerate(parser):
            assert(item['id'] == n)

    with TestCase('ijson.yajl2', 'bytes', args.json_size, results):
        gen = io.BytesIO(jsondata.encode('utf-8'))
        parser = ijson_yajl2.items(gen, 'level1.level2.item')
        for n, item in enumerate(parser):
            assert(item['id'] == n)

    with TestCase('ijson.python', 'str', args.json_size, results):
        gen = io.StringIO(jsondata)
        parser = ijson_python.items(gen, 'level1.level2.item')
        for n, item in enumerate(parser):
            assert(item['id'] == n)

    print(tabulate(
        results,
        headers=['Facility', 'Type', 'Objects/sec'],
        stralign='right',
        # colalign=['left', 'center', 'right'],
        tablefmt='pipe'
    ))
