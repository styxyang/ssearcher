#                                               -*- Autoconf -*-
# Process this file with autoconf to produce a configure script.

AC_PREREQ([2.69])
AC_INIT([ssearcher], [0.1], [hy.styx@gmail.com])
AC_CONFIG_SRCDIR([src/kmp.c])
AC_CONFIG_HEADERS([config.h])

# Checks for programs.
AC_PROG_CXX
AC_PROG_CC

AM_INIT_AUTOMAKE([1.13 foreign subdir-objects])

# Checks for libraries.

# Checks for header files.
AC_CHECK_HEADERS([fcntl.h stdlib.h string.h unistd.h])

# Checks for typedefs, structures, and compiler characteristics.

# Checks for library functions.
AC_FUNC_MALLOC
AC_FUNC_MMAP
AC_CHECK_FUNCS([munmap])

AC_CONFIG_FILES([Makefile])
AC_OUTPUT
