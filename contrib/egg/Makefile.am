CLEANFILES =
DISTCLEANFILES =
EXTRA_DIST =
BUILT_SOURCES =

pkglibdir = $(libdir)/gydict
pkglib_LTLIBRARIES = libegg-private.la

headersdir = $(includedir)/gydict/egg
headers_DATA = \
	egg-menu-manager.h \
	$(NULL)

libegg_private_la_SOURCES = \
	$(headers_DATA) \
	egg-menu-manager.c \
	$(NULL)

libegg_private_la_CFLAGS = \
	$(EGG_CFLAGS) \
	$(NULL)

libegg_private_la_LIBADD = \
	$(EGG_LIBS) \
	$(NULL)

-include $(top_srcdir)/git.mk
