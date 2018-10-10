AC_INIT([u3], [0.1.0], [michal.lyszczek@bofc.pl])
AM_INIT_AUTOMAKE([foreign])
AC_PROG_CC
AC_PROG_LIBTOOL
AC_CONFIG_MACRO_DIR([m4])
AC_CONFIG_FILES([Makefile src/Makefile tst/Makefile inc/Makefile])
AC_CONFIG_SRCDIR([configure.ac])
AC_CONFIG_HEADERS([config.h])

AC_CONFIG_LINKS([tst/mtest.sh:tst/mtest.sh])
AC_CONFIG_LINKS([tst/rev-test.sh:tst/rev-test.sh])

AC_FUNC_MMAP
AC_CHECK_HEADERS([linux/limits.h])

###
# gcov coverage reporting
#


m4_include([m4/gcov.m4])
AC_TDD_GCOV
AC_SUBST(COVERAGE_CFLAGS)
AC_SUBST(COVERAGE_CXXFLAGS)
AC_SUBST(COVERAGE_LDFLAGS)


###
# detect whether HAVE_MUTABLE_STDOUT
#


AC_MSG_CHECKING("whether stdout is modifiable")
AC_TRY_RUN(
[
    #include <stdio.h>
    int main() { FILE *f = NULL; stdout = f; return stdout != NULL; }
]
, # if true
[
    AC_DEFINE([HAVE_MUTABLE_STDOUT], [1], [stdout is mutable])
    AC_MSG_RESULT("yes")
]
, # if false
[
    AC_MSG_RESULT("no")
])


###
# --enable-analyzer
#


AC_ARG_ENABLE([analyzer],
    AS_HELP_STRING([--enable-analyzer], [Enable static code analyzer]),
    [enable_analyzer="yes"], [enable_analyzer="no"])
AM_CONDITIONAL([ENABLE_ANALYZER], [test "x$enable_analyzer" = "xyes"])


###
# --enable-malloc
#


AC_ARG_ENABLE([malloc],
    AS_HELP_STRING([--enable-malloc], [Enable use of malloc]),
    [], [enable_malloc="yes"])

AS_IF([test "x$enable_malloc" = "xyes"],
[
    AC_DEFINE([ENABLE_MALLOC], [1], [Enable use of malloc])
],
# else
[
    enable_malloc="no"
])


###
# --enable-standalone
#


AC_ARG_ENABLE([standalone],
    AS_HELP_STRING([--enable-standalone], [Enable use of malloc]),
    [], [enable_standalone="yes"])

AM_CONDITIONAL([ENABLE_STANDALONE], [test "x$enable_standalone" = "xyes"])
AS_IF([test "x$enable_standalone" = "xyes"],
[
    AC_DEFINE([ENABLE_STANDALONE], [1], [Enable use of standalone])
],
# else
[
    enable_standalone="no"
])


###
# --enable-library
#


AC_ARG_ENABLE([library],
    AS_HELP_STRING([--enable-library], [Enable use of malloc]),
    [], [enable_library="yes"])

AM_CONDITIONAL([ENABLE_LIBRARY], [test "x$enable_library" = "xyes"])
AS_IF([test "x$enable_library" = "xyes"],
[
    AC_DEFINE([ENABLE_LIBRARY], [1], [Enable use of library])
],
# else
[
    enable_library="no"
])


###
# VARIABLE=value options
#

###
# U3_REV_LINE_MAX
#

AC_ARG_VAR([U3_REV_LINE_MAX], [Max size of line buffer])
AS_IF([test "x$U3_REV_LINE_MAX" = "x"], [U3_REV_LINE_MAX="256"])
AC_DEFINE_UNQUOTED([U3_REV_LINE_MAX], [$U3_REV_LINE_MAX], [Max size of line buffer])

AC_OUTPUT

echo
echo "u3 compilation configuration summary"
echo "build standalone...,...: $enable_standalone"
echo "build library..........: $enable_library"
echo ""
echo "enable malloc..........: $enable_malloc"
echo ""
echo "rev: line max..........: $U3_REV_LINE_MAX"
