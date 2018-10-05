include Makefile.am.coverage
ACLOCAL_AMFLAGS=-I m4

SUBDIRS = src tst inc
EXTRA_DIST = tap-driver.sh

if HAVE_GCOV
clean-local: clean-gcov
endif

analyze:
	make analyze -C src
