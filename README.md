# JsonSlicer - stream JSON parser

<a href="https://repology.org/metapackage/python:jsonslicer/versions">
	<img src="https://repology.org/badge/vertical-allrepos/python:jsonslicer.svg" alt="py-jsonslicer packaging status" align="right">
</a>

[![Build Status](https://travis-ci.org/repology/py-jsonslicer.svg?branch=master)](https://travis-ci.org/repology/py-jsonslicer)
[![PyPI downloads](https://img.shields.io/pypi/dm/jsonslicer.svg)](https://pypi.org/project/jsonslicer/)
[![PyPI version](https://img.shields.io/pypi/v/jsonslicer.svg)](https://pypi.org/project/jsonslicer/)
[![PyPI pythons](https://img.shields.io/pypi/pyversions/jsonslicer.svg)](https://pypi.org/project/jsonslicer/)

## Overview

JsonSlicer performs a **stream** JSON parsing which means it **does
not load** whole JSON into memory and is able to parse **very large**
JSON files or streams. The module is written in C and uses
[YAJL](https://lloyd.github.io/yajl/) JSON parsing library, so it's
also quite fast.

JsonSlicer takes a path of JSON map keys or array indexes, and
extracts data under that path in form of complete Python object.
For instance, for JSON

```json
{
	"animals": {
		"dogs": [
			"sparky",
			"barky"
		]
	}
}
```

for path `(b'animals', b'dogs', 1)` JsonSlicer would return `b'barky'`,
and for path `(b'animals', b'dogs')` `[b'sparky', b'barky']` would
be returned. Wildcards are allowed in paths in form of `None` items,
which would match any map key or array index in the corresponding place.
So for path `(b'animals', b'dogs', None)` JsonSlicer would return
`b'sparky'` and `b'barky'` as two separate objects.

Each object is returned in a form of tuple containing a complete
path and a returned object as the last element. Here's complete
data the examples above would return:

```python
# path (b'animals', b'dogs', 1)
(b'animals', b'dogs', 1, b'barky')

# path (b'animals', b'dogs')
(b'animals', b'dogs', [b'sparky', b'barky'])

# path (b'animals', b'dogs', None)
(b'animals', b'dogs', 0, b'sparky')
(b'animals', b'dogs', 1, b'barky')
```

JsonSlicer itself provides an iterator interface.

Summarizing, to use JsonWriter you just provide an I/O handle and
JSON path/pattern to it, iterate over it and process paths extracted
objects and optional path information as you see fit.

## Example

Consider a very large JSON list of people:

```json
{
	"people": [
		{"name": "John", "age": 31},
		{"name": "Ivan", "age": 26},
		{"name": "Angela", "age": 33},
		...
	]
}
```

Here's how you can work with it with JsonSlicer:

```python
from jsonslicer import JsonSlicer

#
# Iterate over all people
#
with open('people.json', 'b') as data:
	for *path, person in JsonSlicer(data, (b'people', None)):
		print(item)
		# {b'name': b'John', b'age': 31}
		# {b'name': b'Ivan', b'age': 26}
		# {b'name': b'Angela', b'age': 33}
		# ...

#
# extract 3'th person (e.g. at index 2) from the file
#
with open('people.json', 'b') as data:
	*_, person = next(JsonSlicer(data, (b'people', 2)))
	# note that you may ignore path by using _ placeholder

#
# extract highest age from the list
#
with open('people.json', 'b') as data:
	max_age = max((age for *_, age in JsonSlicer(data, (b'people', None, b'age'))))
```

## Status/TODO

JsonSlicer is currently in alpha stage, passing tests but pending
code safety checks and improvements. Also, the following mandatory
features are planned to be implemented to consider the module ready
to use:

- Allow to pass YAJL flags the constuctor
- Allow to transparently operate on text I/O handles (in addition
  to bytes I/O) and return text data (instead of bytes) with specified
  encoding
- Allow simplified output format, e.g. returning just objects or
  key/object pairs instead of complete paths

## Requirements

- Python 3.5+
- pkg-config
- [yajl](https://lloyd.github.io/yajl/) JSON pasing library

## License

MIT license, copyright (c) 2019 Dmitry Marakasov amdmi3@amdmi3.ru.
