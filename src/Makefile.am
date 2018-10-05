if ENABLE_STANDALONE

bin_PROGRAMS = rev

rev_SOURCES = rev.c
rev_CFLAGS = $(COVERAGE_CFLAGS) -I$(top_srcdir)/inc -DU3_STANDALONE=1
rev_LDFLAGS = $(COVERAGE_LDFLAGS)

endif # ENABLE_STANDALONE

if ENABLE_LIBRARY

lib_LTLIBRARIES = libu3.la

libu3_la_SOURCES = rev.c
libu3_la_SOURCES += u3defs.h
libu3_la_CFLAGS = $(COVERAGE_CFLAGS) -I$(top_srcdir)/inc -DU3_LIBRARY=1
libu3_la_LDFLAGS = $(COVERAGE_LDFLAGS) -version-info 1:0:1

endif # ENABLE_LIBRARY

# static code analyzer

if ENABLE_ANALYZER

source = rev.c

analyze_plists = $(source:%.c=%.plist)
MOSTLYCLEANFILES = $(analyze_plists)

$(analyze_plists): %.plist: %.c
	@echo "  CCSA  " $@
	@clang --analyze -DHAVE_CONFIG_H=1 -I.. -I../inc $< -o $@

analyze: $(analyze_plists)

endif
