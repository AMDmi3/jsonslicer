#!/usr/bin/env python3

import subprocess
import sys
from os import path

from setuptools import Extension, setup


here = path.abspath(path.dirname(__file__))

version = '0.1.7'


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


def pkgconfig_yajl():
    try:
        return pkgconfig('yajl')
    except subprocess.CalledProcessError:
        print('MISSING DEPENDENCY: yajl library not found, please install it to continue', file=sys.stderr)
        print('\nSee http://lloyd.github.io/yajl/ for installation instructions, or install', file=sys.stderr)
        print('it from your package manager', file=sys.stderr)
        sys.exit(1)
    except FileNotFoundError:
        print('MISSING DEPENDENCY: pkg-config not found, please install it to continue', file=sys.stderr)
        print('\nSee https://www.freedesktop.org/wiki/Software/pkg-config/ for installation', file=sys.stderr)
        print('instructions, or install it from your package manager', file=sys.stderr)
        sys.exit(1)


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
        'Programming Language :: Python :: 3.8',
        'Programming Language :: Python :: 3.9',
        'Programming Language :: Python :: 3.10',
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
            **pkgconfig_yajl()
        )
    ],
    test_suite='tests'
)
