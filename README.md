# jsonslicer - stream JSON parser

<a href="https://repology.org/metapackage/python:jsonslicer/versions">
	<img src="https://repology.org/badge/vertical-allrepos/python:jsonslicer.svg" alt="jsonslicer packaging status" align="right">
</a>

[![CI](https://github.com/AMDmi3/jsonslicer/actions/workflows/ci.yml/badge.svg)](https://github.com/AMDmi3/jsonslicer/actions/workflows/ci.yml)
[![codecov](https://codecov.io/gh/AMDmi3/jsonslicer/branch/master/graph/badge.svg?token=LUBcpfIgCr)](https://codecov.io/gh/AMDmi3/jsonslicer)
[![PyPI downloads](https://img.shields.io/pypi/dm/jsonslicer.svg)](https://pypi.org/project/jsonslicer/)
[![PyPI version](https://img.shields.io/pypi/v/jsonslicer.svg)](https://pypi.org/project/jsonslicer/)
[![PyPI pythons](https://img.shields.io/pypi/pyversions/jsonslicer.svg)](https://pypi.org/project/jsonslicer/)
[![Github commits (since latest release)](https://img.shields.io/github/commits-since/AMDmi3/jsonslicer/latest.svg)](https://github.com/AMDmi3/jsonslicer)

## Overview

JsonSlicer performs a **stream** or **iterative**, **pull** JSON
parsing, which means it **does not load** whole JSON into memory
and is able to parse **very large** JSON files or streams.  The
module is written in C and uses [YAJL](https://lloyd.github.io/yajl/)
JSON parsing library, so it's also quite **fast**.

JsonSlicer takes a **path** of JSON map keys or array indexes, and
provides **iterator interface** which yields JSON data matching
given path as complete Python objects.

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
with open('people.json') as data:
    ivans_age = next(JsonSlicer(data, ('friends', 1, 'age')))
    # 26

with open('people.json') as data:
    managers_name = next(JsonSlicer(data, ('colleagues', 'manager', 'name')))
    # 'Jack'

# Iterate over collection(s) by using wildcards in the path:
with open('people.json') as data:
    for person in JsonSlicer(data, ('friends', None)):
        print(person)
        # {'name': 'John', 'age': 31}
        # {'name': 'Ivan', 'age': 26}

# Iteration over both arrays and dicts is possible, even at the same time
with open('people.json') as data:
    for person in JsonSlicer(data, (None, None)):
        print(person)
        # {'name': 'John', 'age': 31}
        # {'name': 'Ivan', 'age': 26}
        # {'name': 'Jack', 'age': 33}
        # {'name': 'Lucy', 'age': 21}

# Map key of returned objects is available on demand...
with open('people.json') as data:
    for position, person in JsonSlicer(data, ('colleagues', None), path_mode='map_keys'):
        print(position, person)
        # 'manager' {'name': 'Jack', 'age': 33}
        # 'subordinate' {'name': 'Lucy', 'age': 21}

# ...as well as complete path information
with open('people.json') as data:
    for *path, person in JsonSlicer(data, (None, None), path_mode='full'):
        print(path, person)
        # ('friends', 0) {'name': 'John', 'age': 31})
        # ('friends', 1) {'name': 'Ivan', 'age': 26})
        # ('colleagues', 'manager') {'name': 'Jack', 'age': 33})
        # ('colleagues', 'subordinate') {'name': 'Lucy', 'age': 21})

# Extract all instances of deep nested field
with open('people.json') as data:
    age_sum = sum(JsonSlicer(data, (None, None, 'age')))
    # 111
```

## API

```
jsonslicer.JsonSlicer(
    file,
    path_prefix,
    read_size=1024,
    path_mode=None,
    yajl_allow_comments=False,
    yajl_dont_validate_strings=False,
    yajl_allow_trailing_garbage=False,
    yajl_allow_multiple_values=False,
    yajl_allow_partial_values=False,
    yajl_verbose_errors=True,
    encoding=None,
    errors=None,
    binary=False,
)
```

Constructs iterative JSON parser. which reads JSON data from _file_ (a `.read()`-supporting [file-like object](https://docs.python.org/3/glossary.html#term-file-like-object) containing a JSON document).

_file_ is a `.read()`-supporting [file-like
object](https://docs.python.org/3/glossary.html#term-file-like-object)
containing a JSON document. Both binary and text files are supported,
but binary ones are preferred, because the parser has to operate on
binary data internally anyway, and using text input would require an
unnecessary encoding/decoding which yields ~3% performance overhead.
Note that JsonSlicer supports both unicode and binary output regardless
of input format.

_path_prefix_ is an iterable (usually a list or a tuple) specifying
a path or a path pattern of objects which the parser should extract
from JSON.

For instance, in the example above a path `('friends', 0, 'name')`
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

Both strings and byte objects are allowed in path, regardless of
input and output encodings.  are automatically converted
to the format used internally.

_read_size_ is a size of block read by the parser at a time.

_path_mode_ is a string which specifies how a parser should
return path information along with objects. The following modes are
supported:

* _'ignore'_ (the default) - do not output any path information, just
objects as is (`'friends'`).

  ```python
  {'name': 'John', 'age': 31}
  {'name': 'Ivan', 'age': 26}
  {'name': 'Jack', 'age': 33}
  {'name': 'Lucy', 'age': 21}
  ```

  Common usage pattern for this mode is

  ```python
  for object in JsonSlicer(...)
  ```

* _'map_keys'_ - output objects as is when traversing arrays and tuples
consisting of map key and object when traversing maps.

  ```python
  {'name': 'John', 'age': 31}
  {'name': 'Ivan', 'age': 26}
  ('manager', {'name': 'Jack', 'age': 33})
  ('subordinate', {'name': 'Lucy', 'age': 21})
  ```

  This format may seem inconsistent (and therefore it's not the default),
  however in practice only collection of a single type is iterated at
  a time and this type is known, so this format is likely the most useful
  as in most cases you do need dictionary keys.

  Common usage pattern for this mode is

  ```python
  for object in JsonSlicer(...)  # when iterating arrays
  for key object in JsonSlicer(...)  # when iterating maps
  ```

* _'full_paths'_ - output tuples consisting of all path components
(both map keys and array indexes) and an object as the last element.

  ```python
  ('friends', 0, {'name': 'John', 'age': 31})
  ('friends', 1, {'name': 'Ivan', 'age': 26})
  ('colleagues', 'manager', {'name': 'Jack', 'age': 33})
  ('colleagues', 'subordinate', {'name': 'Lucy', 'age': 21})
  ```

  Common usage pattern for this mode is

  ```python
  for *path, object in JsonSlicer(...)
  ```

_yajl_allow_comments_ enables corresponding YAJL flag, which is
documented as follows:

> Ignore javascript style comments present in JSON input.  Non-standard,
> but rather fun

_yajl_dont_validate_strings_ enables corresponding YAJL flag, which
is documented as follows:

> When set the parser will verify that all strings in JSON input
> are valid UTF8 and will emit a parse error if this is not so.  When
> set, this option makes parsing slightly more expensive (~7% depending
> on processor and compiler in use)

_yajl_allow_trailing_garbage_ enables corresponding YAJL flag, which
is documented as follows:

> By default, yajl will ensure the entire input text was consumed
> and will raise an error otherwise.  Enabling this flag will cause
> yajl to disable this check.  This can be useful when parsing json
> out of a that contains more than a single JSON document.

_yajl_allow_multiple_values_ enables corresponding YAJL flag, which
is documented as follows:

> Allow multiple values to be parsed by a single handle.  The entire
> text must be valid JSON, and values can be seperated by any kind
> of whitespace.  This flag will change the behavior of the parser,
> and cause it continue parsing after a value is parsed, rather than
> transitioning into a complete state.  This option can be useful
> when parsing multiple values from an input stream.

_yajl_allow_partial_values_ enables corresponding YAJL flag, which
is documented as follows:

> When yajl_complete_parse() is called the parser will check that the
> top level value was completely consumed.  I.E., if called whilst
> in the middle of parsing a value yajl will enter an error state
> (premature EOF).  Setting this flag suppresses that check and the
> corresponding error.

_yajl_verbose_errors_ enables verbose YAJL errors, with exception
message including the JSON text where the error occured, along with
an arrow pointing to the specific char.

_encoding_ may be used to override output encoding, which is derived
from the input file handle if possible, or otherwise set to the
default one as Python builtin `open()` would use (usually `'UTF-8'`).

_errors_ is an optional string that specifies how encoding and
decoding errors are to be handled. Defaults to `'strict'`

_binary_ forces the output to be in form of `bytes` objects instead
of `str` unicode strings.

The constructed object is as iterator. You may call `next()` to extract
single element from it, iterate it via `for` loop, or use it in generator
comprehensions or in any place where iterator is accepted.

## Performance/competitors

The closest competitor is [ijson](https://github.com/isagalaev/ijson),
and JsonSlicer was written to be better. Namely,

* It's up to 35x faster depending on ijson backend (starting with 3.0,
  ijson supports comparable performance via yajl2_c backend), close in
  performance to Python's native `json` module.
* It supports more flexible paths/patterns specifying which objects
  to iterate over in JSON hierarchy and provides consistent interface
  for iteration over arrays and dictionaries

The results of bundled benchmark on Python 3.8.2 / clang 8.0.1 / `-O2 -DNDEBUG` / FreeBSD 12.1 amd64 / Core i7-6600U CPU @ 2.60GHz.

|                                                 Facility |   Type |   Objects/sec |
|---------------------------------------------------------:|-------:|--------------:|
|                                             json.loads() |    str |       1147.6K |
|                                    json.load(StringIO()) |    str |       1139.3K |
|   **JsonSlicer (no paths, binary input, binary output)** |  bytes |       1149.7K |
|  **JsonSlicer (no paths, unicode input, binary output)** |  bytes |       1134.5K |
|  **JsonSlicer (no paths, binary input, unicode output)** |    str |       1012.3K |
| **JsonSlicer (no paths, unicode input, unicode output)** |    str |        996.2K |
|               **JsonSlicer (full paths, binary output)** |  bytes |        763.1K |
|              **JsonSlicer (full paths, unicode output)** |    str |        567.2K |
|                                            ijson.yajl2_c |  bytes |       1062.0K |
|                                         ijson.yajl2_cffi |  bytes |         71.6K |
|                                              ijson.yajl2 |  bytes |         56.4K |
|                                             ijson.python |    str |         32.0K |

## Status/TODO

JsonSlicer is currently in beta stage, used in production in
[Repology](https://repology.org) project. Testing foci are:

- Edge cases with uncommon encoding (input/output) configurations
- Absence of memory leaks

## Requirements

- Python 3.4+
- pkg-config
- [yajl](https://lloyd.github.io/yajl/) 2.0.3+ (older versions lack pkgconfig file)

## License

MIT license, copyright (c) 2019 Dmitry Marakasov amdmi3@amdmi3.ru.
