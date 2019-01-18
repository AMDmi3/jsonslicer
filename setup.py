#!/usr/bin/env python3

import subprocess
from os import path

from setuptools import Extension, setup


here = path.abspath(path.dirname(__file__))


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


def get_version():
    # XXX: implement __version__ in the module and read it here
    # with open(path.join(here, 'jsonslicer', '__init__.py')) as source:
    #    for line in source:
    #        if line.startswith('__version__'):
    #            return line.strip().split(' = ')[-1].strip('\'')

    return '0.0.1'

    raise RuntimeError('Cannot determine package version from package source')


def get_long_description():
    try:
        return open(path.join(here, 'README.md')).read()
    except:
        return None


setup(
    name='jsonslicer',
    version=get_version(),
    description='Stream JSON parser with iterator interface',
    long_description=get_long_description(),
    long_description_content_type='text/markdown',
    author='Dmitry Marakasov',
    author_email='amdmi3@amdmi3.ru',
    url='https://github.com/AMDmi3/jsonslicer',
    license='MIT',
    keywords=['json', 'parser', 'pull', 'stream'],
    classifiers=[
        'Development Status :: 2 - Pre-Alpha',
        'Intended Audience :: Developers',
        'License :: OSI Approved :: MIT License',
        'Programming Language :: C',
        'Programming Language :: Python :: 3 :: Only',
        'Programming Language :: Python :: 3.6',
        'Programming Language :: Python :: 3.7',
    ],
    ext_modules=[
        Extension(
            'jsonslicer',
            sources=[
                'src/construct_handlers.c',
                'src/handlers.c',
                'src/jsonslicer.c',
                'src/output_formatting.c',
                'src/py_module.c',
                'src/pymutindex.c',
                'src/pyobjlist.c',
                'src/seek_handlers.c',
            ],
            **pkgconfig('yajl')
        )
    ],
    test_suite='tests'
)
