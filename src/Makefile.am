include ../Makefile.am.coverage

bin_PROGRAMS = rev
rev_SOURCES = rev.c
rev_CFLAGS = $(COVERAGE_CFLAGS)
rev_LDFLAGS = $(COVERAGE_LDFLAGS)

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
