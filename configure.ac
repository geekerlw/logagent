#                                               -*- Autoconf -*-
# Process this file with autoconf to produce a configure script.

AC_PREREQ([2.69])
AC_INIT([logagent], [0.1], [https://github.com/geekerlw/aliyun-log-c-agent.git])
AM_INIT_AUTOMAKE([1.0 subdir-objects])
AC_CONFIG_SRCDIR([src/logagent.c])
AC_CONFIG_HEADERS([config.h])

# Checks for programs.
AC_PROG_CC
AC_PROG_CXX
AC_PROG_LIBTOOL

# Libtool initial
LT_PREREQ([2.2.6])
LT_INIT([disable-static])

# Checks for tools
AC_CHECK_TOOL([PKG_CONFIG], [pkg-config])

# Checks for libraries.
PKG_CHECK_MODULES(JSON_C, json-c >= 0.11.99)
AC_SUBST(JSON_C_CFLAGS)
AC_SUBST(JSON_C_LIBS)

PKG_CHECK_MODULES(APR, apr-1 >= 1.5.2)
AC_SUBST(APR_CFLAGS)
AC_SUBST(APR_LIBS)

PKG_CHECK_MODULES(APR_UTIL, apr-util-1 >= 1.5.4)
AC_SUBST(APR_UTIL_CFLAGS)
AC_SUBST(APR_UTIL_LIBS)

PKG_CHECK_MODULES(LIBCURL, libcurl >= 7.47.0)
AC_SUBST(LIBCURL_CFLAGS)
AC_SUBST(LIBCURL_LIBS)

AC_SUBST(MATH_LIBS, -lm)
AC_SUBST(DLL_LIBS, -ldl)
AC_SUBST(PTHREAD_LIBS, -lpthread)

# Checks for header files.
AC_CHECK_HEADERS([unistd.h, pthread.h])

# Checks for typedefs, structures, and compiler characteristics.
AC_CHECK_HEADER_STDBOOL
AC_C_INLINE

# Checks for library functions.
AC_FUNC_MALLOC
AC_CHECK_FUNCS([memset])

AC_CONFIG_FILES([
Makefile
src/Makefile
plugins/Makefile
])

AC_OUTPUT
