PYTHON?=	python3
FLAKE8?=	flake8
TWINE?=		twine

all: build

lint: flake8

build:
	python3 setup.py build

clean:
	rm -rf build

test:
	python3 -m unittest discover

flake8:
	${FLAKE8} *.py tests

sdist::
	${PYTHON} setup.py sdist

release::
	rm -rf dist
	${PYTHON} setup.py sdist
	${TWINE} upload dist/*.tar.gz
