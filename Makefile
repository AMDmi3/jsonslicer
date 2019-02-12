FLAKE8?=	flake8

test::
	python3 -m unittest discover

flake8:
	# D10  - Missing docstrings
	# E501 - Line too long
	# E722 - Do not use bare except
	${FLAKE8} --ignore=D10,E501,E722 --count *.py tests

flake8-all:
	${FLAKE8} *.py tests
