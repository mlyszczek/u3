check_PROGRAMS = rev-test
dist_check_SCRIPTS = rev-test.sh

rev_test_SOURCES = $(sources_common) rev-test.c


include_common = mtest.h std-redirects.h
sources_common = std-redirects.c $(include_common)

CFLAGS = -I$(top_srcdir)/src -I$(top_srcdir)/tst -I$(top_srcdir)/inc \
	-DU3_STANDALONE=1 $(COVERAGE_CFLAGS)
LDFLAGS = -static -L$(top_builddir)/src/.libs $(COVERAGE_LDFLAGS)
LDADD = -lu3

TESTS = $(check_PROGRAMS) $(dist_check_SCRIPTS)
LOG_DRIVER = env AM_TAP_AWK='$(AWK)' $(SHELL) \
	$(top_srcdir)/tap-driver.sh
