lib_LTLIBRARIES = liblist.la

#noinst_HEADERS = liblist.la
liblist_la_LDFLAGS = \
					-module	\
					-shared	\
					-prefer-pic	\
					-version-info 0:0:0	\
					-rpath /usr/local/lib

liblist_la_CFLAGS = \
					-I../	\
					-I../lib	\
					-shared	\
					-fPIC	\
					$(CFLAGS)

liblist_la_SOURCES = \
					 list.c list.h


#liblist_la_SOURCES = list.c
##liblist_la_LIBADD =
#
#noinst_HEADERS = list.h
#
#AM_CFLAGS = -Wall $(LIST_IN_C_CFLAGS)
#
#CLEANFILES =
