check_PROGRAMS = test

test_SOURCES = rev_test.c main.c std-redirects.c mtest.h std-redirects.h
test_CFLAGS = -I$(top_srcdir)/src -I$(top_srcdir)/tst -I$(top_srcdir)/inc
test_LDFLAGS = -static -L$(top_srcdir)/src/.libs
test_LDADD = -lu3

TESTS = $(check_PROGRAMS)
LOG_DRIVER = env AM_TAP_AWK='$(AWK)' $(SHELL) \
	$(top_srcdir)/tap-driver.sh
