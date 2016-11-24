#
#
#
INCLUDES = -I$(top_srcdir) -I@apache_dir@/include

AM_CPPFLAGS = -D_LARGEFILE_SOURCE -D_FILE_OFFSET_BITS=64 -D_LARGEFILE64_SOURCE

lib_LTLIBRARIES = libmodreceta.la
libmodreceta_la_LIBADD = -lmysqlclient
libmodreceta_la_SOURCES = logp.h \
	mod_receta.h mod_receta.c \
	sql.h sql.c

if DEBUG
libmodreceta_la_CFLAGS = -Wall -g
libmodreceta_la_CPPFLAGS = $(AM_CPPFLAGS) -DDEBUG
endif
