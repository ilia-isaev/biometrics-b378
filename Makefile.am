AM_CPPFLAGS  = -Wall -Werror

EXTRA_DIST  = autogen.sh TIPS

lib_LTLIBRARIES = lib378.la
lib378_la_SOURCES = \
		src/lib378.c     src/lib378.h

include_HEADERS = \
		src/lib378.h

bin_PROGRAMS = b378
b378_SOURCES = \
		src/main.c \
		src/ftool.c      src/ftool.h

b378_LDADD  = lib378.la

