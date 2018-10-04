include ../Makefile.am.coverage

if ENABLE_STANDALONE

bin_PROGRAMS = rev
rev_SOURCES = rev.c
rev_CFLAGS = $(COVERAGE_CFLAGS) -I$(top_srcdir)/inc
rev_LDFLAGS = $(COVERAGE_LDFLAGS)

else # ENABLE_STANDALONE

lib_LTLIBRARIES = libu3.la

libu3_la_SOURCES = rev.c
libu3_la_SOURCES += u3.h utils.h
libu3_la_CFLAGS = $(COVERAGE_CFLAGS) -I$(top_srcdir)/inc
libu3_la_LDFLAGS = $(COVERAGE_LDFLAGS) -version-info 1:0:0

endif # ENABLE_STANDALONE


# static code analyzer

if ENABLE_ANALYZER

source = rev.c

analyze_plists = $(source:%.c=%.plist)
MOSTLYCLEANFILES = $(analyze_plists)

$(analyze_plists): %.plist: %.c
	@echo "  CCSA  " $@
	@clang --analyze -I.. $< -o $@

analyze: $(analyze_plists)

endif
