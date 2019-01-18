# jsonslicer - stream JSON parser

<a href="https://repology.org/metapackage/python:jsonslicer/versions">
	<img src="https://repology.org/badge/vertical-allrepos/python:jsonslicer.svg" alt="jsonslicer packaging status" align="right">
</a>

[![Build Status](https://travis-ci.org/AMDmi3/jsonslicer.svg?branch=master)](https://travis-ci.org/AMDmi3/jsonslicer)
[![Coverage Status](https://coveralls.io/repos/github/AMDmi3/jsonslicer/badge.svg?branch=master)](https://coveralls.io/github/AMDmi3/jsonslicer?branch=master)
[![PyPI downloads](https://img.shields.io/pypi/dm/jsonslicer.svg)](https://pypi.org/project/jsonslicer/)
[![PyPI version](https://img.shields.io/pypi/v/jsonslicer.svg)](https://pypi.org/project/jsonslicer/)
[![PyPI pythons](https://img.shields.io/pypi/pyversions/jsonslicer.svg)](https://pypi.org/project/jsonslicer/)

## Overview

JsonSlicer performs a **stream** or **iterative** JSON parsing,
which means it **does not load** whole JSON into memory and is able
to parse **very large** JSON files or streams. The module is written
in C and uses [YAJL](https://lloyd.github.io/yajl/) JSON parsing
library, so it's also quite **fast**.

JsonSlicer takes a path of JSON map keys or array indexes, and provides
iterator interface which yields JSON data matching given path as complete
Python objects.

## Example

```json
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
```

```python
from jsonslicer import JsonSlicer

# Extract specific elements:
with open('people.json', 'b') as data:
	ivans_age = next(JsonSlicer(data, (b'friends', 1, b'age')))

with open('people.json', 'b') as data:
	managers_name = next(JsonSlicer(data, (b'collegues', b'manager', b'name')))

# Iterate over collection(s) by using wildcards in the path:
with open('people.json', 'b') as data:
	for person in JsonSlicer(data, (b"friends", None)):
		print(person)
		# {b'name': b'John', b'age': 31}
		# {b'name': b'Ivan', b'age': 26}

# Uniform iteration is possible
with open('people.json', 'b') as data:
	for person in JsonSlicer(data, (None, None)):
		print(person)
		# {b'name': b'John', b'age': 31}
		# {b'name': b'Ivan', b'age': 26}
		# {b'name': b'Jack', b'age': 33}
		# {b'name': b'Lucy', b'age': 21}

# Map key of returned objects is available on demand...
with open('people.json', 'b') as data:
	for position, person in JsonSlicer(data, ('colleagues', None), path_format='map_keys'):
		print(position, person)
		# b'manager' {b'name': b'Jack', b'age': 33}
		# b'subordinate' {b'name': b'Lucy', b'age': 21}

# ...as well as complete path information
with open('people.json', 'b') as data:
	for person in JsonSlicer(data, (None, None), path_format='full'):
		print(person)
		# (b'friends', 0, {b'name': b'John', b'age': 31})
		# (b'friends', 1, {b'name': b'Ivan', b'age': 26})
		# (b'colleagues', b'manager', {b'name': b'Jack', b'age': 33})
		# (b'colleagues', b'subordinate', {b'name': b'Lucy', b'age': 21})

# Extract all instances of deep nested field
with open('people.json', 'b') as data:
	max_age = max(JsonSlicer(data, (b'people', None, b'age')))
	# 33
```

## API

```
jsonslicer.JsonSlicer(file, path_prefix, read_size=1024, path_mode=None)
```

Constructs iterative JSON parser which reads JSON data from _file_ (a `.read()`-supporting [file-like object](https://docs.python.org/3/glossary.html#term-file-like-object) containing a JSON document).

_path_prefix_ is an iterable (usually a list or a tuple) specifying
a path or a path pattern of objects which the parser should extract
from JSON.

For instance, in the example above a path `(b'friends', 0, b'name')`
will yield string `'John'`, by descending from the root element
into the dictionary element by key `'friends'`, then into the array
element by index `0`, then into the dictionary element by key
`'name'`. Note that integers only match array indexes and strings
only match dictionary keys.

The path can be turned into a pattern by specifying `None` as a
placeholder in some path positions. For instance,  `(None, None,
'name')` will yield all four names from the example above, because
it matches an item under 'name' key on the second nesting level of
any arrays or map structure.

_read_size_ is a size of block read by the parser at a time.

_path_mode_ is a string which specifies how a parser should
return path information along with objects. The following modes are
supported:

* _ignore_ (the default) - do not output any path information, just
objects as is (`b'friends'`).

  ```python
  {b'name': b'John', b'age': 31}
  {b'name': b'Ivan', b'age': 26}
  {b'name': b'Jack', b'age': 33}
  {b'name': b'Lucy', b'age': 21}
  ```

  Common usage pattern for this mode is `for object in JsonWriter(...)`

* _map_keys_ - output objects as is when traversing arrays and tuples
consisting of map key and object when traversing maps.

  ```python
  {b'name': b'John', b'age': 31}
  {b'name': b'Ivan', b'age': 26}
  (b'manager', {b'name': b'Jack', b'age': 33})
  (b'subordinate', {b'name': b'Lucy', b'age': 21})
  ```

  This format may seem inconsistent (and therefore it's not the default),
  however in practice only collection of a single type is iterated at
  a time and this type is known, so this format is likely the most useful
  as in most cases you do need dictionary keys. Common

  Common usage pattern for this mode is `for object in JsonSlicer(...)`
  when iterating arrays and `for key object in JsonSlicer(...)`
  when iterating maps.

* _full_paths_ - output tuples consisting of all path components
(both map keys and array indexes) and an object as the last element.

  ```python
  (b'friends', 0, {b'name': b'John', b'age': 31})
  (b'friends', 1, {b'name': b'Ivan', b'age': 26})
  (b'colleagues', b'manager', {b'name': b'Jack', b'age': 33})
  (b'colleagues', b'subordinate', {b'name': b'Lucy', b'age': 21})
  ```

  Common usage pattern for this mode is `for *path, object in JsonWriter(...)`.

The constructed object is as iterator. You may call `next()` to extract
single element from it, iterate it via `for` loop, or use it in generator
comprehensions or in any place where iterator is accepted.

## Performance/competitors

The closest competitor is [ijson](https://github.com/isagalaev/ijson),
and JsonSlicer was written to be better. Namely,

* It's about 10x faster, pretty close to Python's native `json` module
* It allows iterating over dictionaries and allows more flexibility when
  specifying paths/patterns of objects to iterate over

The results of bundled benchmark on Python 3.7 / clang 6.0.1 / FreeBSD 12.0 amd64 / Core i7-6600U CPU @ 2.60GHz.

| Facility              | Type   | Objects/sec   |
|-----------------------|:------:|--------------:|
| json.loads()          |    str |       1115.5K |
| json.load(StringIO()) |    str |       1042.9K |
| **JsonSlicer**        |  bytes |        822.5K |
| ijson.yajl2_cffi      |  bytes |         72.0K |
| ijson.yajl2           |  bytes |         44.7K |
| ijson.python          |    str |         28.4K |

## Status/TODO

JsonSlicer is currently in alpha stage, passing tests but pending
code safety checks and improvements. Also, the following mandatory
features are planned to be implemented to consider the module ready
to use:

- Allow to pass YAJL flags the constuctor
- Allow to transparently operate on text I/O handles (in addition
  to bytes I/O) and return text data (instead of bytes) with specified
  encoding

## Requirements

- Python 3.6+
- pkg-config
- [yajl](https://lloyd.github.io/yajl/) JSON pasing library

## License

MIT license, copyright (c) 2019 Dmitry Marakasov amdmi3@amdmi3.ru.
