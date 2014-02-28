
dnl Process this file with autoconf to produce a configure script.

AC_INIT([neutx], [0.1])
AC_CONFIG_AUX_DIR([build-aux])
AC_CONFIG_MACRO_DIR([build-aux])
AM_INIT_AUTOMAKE([-Wall -Werror foreign])
AC_CONFIG_HEADERS([config.h])

dnl Build options.

dnl optional build demo
AC_ARG_ENABLE(demo,
    AC_HELP_STRING([--enable-demo], [compile demo programs [[default=no]]]),
    [case ${enableval} in yes) demo=true;; *) demo=false;; esac], [demo=false]
)
AM_CONDITIONAL([DEMO], [test "x$demo" = "xtrue"])

CXXFLAGS="${CXXFLAGS% } -MMD"

AC_ARG_ENABLE(debug,
    AC_HELP_STRING([--enable-debug], [enable debug [[default=yes]]]),
    [ if test "x$enable_debug" = "xyes" -o -z "x$enable_debug"; then
        CXXFLAGS="${CXXFLAGS% } -g -O0 -Wall"
      fi
    ]
)

AC_ARG_ENABLE(optimize,
    AC_HELP_STRING([--enable-optimize], [enable optimization [[default=no]]]),
    [ if test "x$enable_optimize" = "xyes" ; then
          CXXFLAGS="${CXXFLAGS% } -g -O3"
      fi
    ]
)

AC_ARG_ENABLE(warnings,
    AC_HELP_STRING([--enable-warnings], [enable all warnings [[default=yes]]]),
    [ if test "x$enable_warnings" = "xyes" -o -z "$enable_warnings"; then
          CXXFLAGS="${CXXFLAGS% } -Wall -Werror"
      fi
    ]
)

dnl support static libraries via libtool
AM_PROG_AR

dnl libtool
LT_INIT

dnl Checks for programs.
AC_PROG_CXX

dnl Checks for libraries.

dnl Output.

AC_CONFIG_FILES([Makefile])
AC_CONFIG_FILES([src/Makefile])
AC_CONFIG_FILES([demo/Makefile])

AC_OUTPUT
