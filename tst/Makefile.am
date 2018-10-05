check_PROGRAMS = rev-test
check_SCRIPTS = rev-test.sh

rev_test_SOURCES = rev-test.c main.c std-redirects.c mtest.h std-redirects.h
rev_test_CFLAGS = -I$(top_srcdir)/src -I$(top_srcdir)/tst -I$(top_srcdir)/inc \
	-DU3_STANDALONE=1
rev_test_LDFLAGS = -static -L$(top_srcdir)/src/.libs
rev_test_LDADD = -lu3

TESTS = $(check_PROGRAMS) $(check_SCRIPTS)
LOG_DRIVER = env AM_TAP_AWK='$(AWK)' $(SHELL) \
	$(top_srcdir)/tap-driver.sh
