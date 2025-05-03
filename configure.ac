AC_PREREQ([2.71])
AC_INIT([b378],[0.0.0],[dev@isaev.com.br])
AC_CANONICAL_HOST
AC_CONFIG_MACRO_DIRS([m4])
AC_CONFIG_SRCDIR([src/main.c])
AC_CONFIG_HEADERS([config.h])

# Checks for programs.
AC_PROG_CC

# Checks for libraries.
#AC_LANG([C++])
#AC_CHECK_LIB([stdc++],[__cxa_demangle])
#AC_CHECK_LIB([pthread],[pthread_create],[],[AC_MSG_ERROR([Could not find pthread library])])
AC_CHECK_LIB([m],[cos])

# Checks for header files.
#AC_CHECK_HEADERS([pthread.h],[],[AC_MSG_ERROR([pthread headers are required])])

# Checks for typedefs, structures, and compiler characteristics.
AC_TYPE_SIZE_T
AC_TYPE_SSIZE_T
AC_CHECK_MEMBERS([struct stat.st_blksize])

# Checks for library functions.
AM_PROG_AR
LT_INIT
AM_INIT_AUTOMAKE([subdir-objects -Wall -Werror foreign])
AM_PROG_CC_C_O
AC_CONFIG_FILES([Makefile])
AC_OUTPUT
