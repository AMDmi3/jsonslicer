#!/usr/bin/env python3

import subprocess
from os import path

from setuptools import Extension, setup


here = path.abspath(path.dirname(__file__))

version = '0.1.4'


def pkgconfig(package):
    result = {}
    for token in subprocess.check_output(['pkg-config', '--libs', '--cflags', package]).decode('utf-8').split():
        if token.startswith('-I'):
            result.setdefault('include_dirs', []).append(token[2:])
        elif token.startswith('-L'):
            result.setdefault('library_dirs', []).append(token[2:])
        elif token.startswith('-l'):
            result.setdefault('libraries', []).append(token[2:])
    return result


def get_long_description():
    try:
        return open(path.join(here, 'README.md')).read()
    except:
        return None


setup(
    name='jsonslicer',
    version=version,
    description='Stream JSON parser with iterator interface',
    long_description=get_long_description(),
    long_description_content_type='text/markdown',
    author='Dmitry Marakasov',
    author_email='amdmi3@amdmi3.ru',
    url='https://github.com/AMDmi3/jsonslicer',
    packages=['jsonslicer'],
    package_data={'jsonslicer': ['py.typed', '__init__.pyi']},
    license='MIT',
    keywords=['json', 'parser', 'pull', 'stream'],
    classifiers=[
        'Development Status :: 4 - Beta',
        'Intended Audience :: Developers',
        'License :: OSI Approved :: MIT License',
        'Programming Language :: C++',
        'Programming Language :: Python :: 3 :: Only',
        'Programming Language :: Python :: 3.4',
        'Programming Language :: Python :: 3.5',
        'Programming Language :: Python :: 3.6',
        'Programming Language :: Python :: 3.7',
    ],
    ext_modules=[
        Extension(
            'jsonslicer',
            language='c++',
            extra_compile_args=[
                '-std=c++11',
                '-DJSONSLICER_VERSION=\"{}\"'.format(version),
                # JsonSlicer can operate in two modes, either storing paths
                # in binary or in unicode form internally. The former is
                # slightly faster since we need to compare bytes instead of
                # unicode, both seem to be correct, but I'm leaving the toggle
                # here if some edge cases are discovered.
                '-DUSE_BYTES_INTERNALLY',
                '-fno-exceptions',
                '-fno-rtti',
            ],
            sources=[
                'src/construct_handlers.cc',
                'src/encoding.cc',
                'src/handlers.cc',
                'src/jsonslicer_construction.cc',
                'src/jsonslicer_iteration.cc',
                'src/jsonslicer_type.cc',
                'src/output_formatting.cc',
                'src/py_module.cc',
                'src/pymutindex.cc',
                'src/pyobjlist.cc',
                'src/seek_handlers.cc',
            ],
            **pkgconfig('yajl')
        )
    ],
    test_suite='tests'
)
