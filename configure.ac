
dnl Process this file with autoconf to produce a configure script.

AC_INIT([neutx], [0.1])
AC_CONFIG_AUX_DIR([build-aux])
AC_CONFIG_MACRO_DIR([build-aux])
AM_INIT_AUTOMAKE([-Wall -Werror foreign])
AC_CONFIG_HEADERS([config.h])

dnl save flags before boost tests
CXX_FLAGS_SAVED="${CXXFLAGS}"

dnl boost library
AX_BOOST_BASE
AX_BOOST_CHRONO
AX_BOOST_SYSTEM
AX_BOOST_UNIT_TEST_FRAMEWORK

dnl make use of boost lib dir
AC_SUBST([BOOSTLIBDIR])

dnl find list of all header files in include folder
inc_files_list=`find include -name \*.@<:@hi@:>@pp -print`
AC_SUBST([INCFILES], [`echo $inc_files_list`])

dnl Checks for programs.
AC_PROG_CXX

dnl Support static libraries via libtool
AM_PROG_AR

dnl Libtool
LT_INIT

dnl Checks for libraries.

dnl Tweaking build options.

dnl restore original flags, adding new options
CXXFLAGS="${CXX_FLAGS_SAVED% } -MMD"

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
          CXXFLAGS="${CXXFLAGS% } -g -O3 -fno-strict-aliasing"
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

dnl optional build demo
AC_ARG_ENABLE(demo,
    AC_HELP_STRING([--enable-demo], [compile demo programs [[default=no]]]),
    [case ${enableval} in yes) demo=true;; *) demo=false;; esac], [demo=false]
)
AM_CONDITIONAL([DEMO], [test "x$demo" = "xtrue"])

dnl Output.

AC_CONFIG_FILES([Makefile])
AC_CONFIG_FILES([src/Makefile])
AC_CONFIG_FILES([test/Makefile])
AC_CONFIG_FILES([demo/Makefile])
AC_CONFIG_FILES([test.sh], [chmod +x test.sh])

AC_OUTPUT
