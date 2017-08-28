CLEANFILES =
DISTCLEANFILES =
EXTRA_DIST =
BUILT_SOURCES =

pkglibdir = $(libdir)/gydict
pkglib_LTLIBRARIES = libpattern-private.la

headersdir = $(includedir)/gydict/pattern
headers_DATA = \
	g-observable.h \
	g-observer.h \
	g-pattern.h \
	$(NULL)

libpattern_private_la_SOURCES = \
	$(headers_DATA) \
	g-observable.c \
	g-observer.c \
	$(NULL)

libpattern_private_la_CFLAGS = \
	$(GYDICT_CFLAGS) \
	$(NULL)

libpattern_private_la_LIBADD = \
	$(GYDICT_LIBS) \
	$(NULL)

-include $(top_srcdir)/git.mk
