CLEANFILES =
DISTCLEANFILES =
BUILT_SOURCES =
EXTRA_DIST =

pkglibdir = $(libdir)/gydict
pkglib_LTLIBRARIES = libgydict.la

headersdir = $(includedir)/gydict/libgydict
libgydict_la_public_headers = \
	app/gy-app.h \
	css/gy-css-provider.h \
	dictionaries/gy-depl.h \
	dictionaries/gy-dict.h \
	dictionaries/gy-dict-manager.h \
	dictionaries/gy-english-pwn.h \
	dictionaries/gy-german-pwn.h \
	dictionaries/gy-parsable.h \
	dictionaries/gy-parser-pwn.h \
	dictionaries/gy-pwn-dict.h \
	dictionaries/gy-pwntabs.h \
	entrylist/gy-tree-view.h \
	entryview/gy-text-buffer.h \
	entryview/gy-text-view.h \
	helpers/gy-utility-func.h \
	preferences/gy-preferences-file-chooser.h \
	preferences/gy-preferences-page.h \
	preferences/gy-preferences-page-dicts.h \
	preferences/gy-preferences-page-general.h \
	preferences/gy-preferences-switch.h \
	preferences/gy-preferences-window.h \
	printing/gy-print.h \
	printing/gy-print-compositor.h \
	search/gy-search-bar.h \
	shortcuts/gy-shortcuts-window.h \
	window/gy-header-bar.h \
	window/gy-window-settings.h \
	window/gy-window.h \
	window/gy-workspace.h \
	gydict.h \
	$(NULL)

libgydict_la_SOURCES = \
	$(libgydict_la_public_headers) \
	app/gy-app.c \
	css/gy-css-provider.c \
	dictionaries/gy-depl.c \
	dictionaries/gy-dict.c \
	dictionaries/gy-dict-manager.c \
	dictionaries/gy-english-pwn.c \
	dictionaries/gy-german-pwn.c \
	dictionaries/gy-parsable.c \
	dictionaries/gy-parser-pwn.c \
	dictionaries/gy-pwn-dict.c \
	dictionaries/gy-pwntabs.c \
	entrylist/gy-tree-view.c \
	entryview/gy-text-buffer.c \
	entryview/gy-text-buffer-private.h \
	entryview/gy-text-view.c \
	helpers/gy-utility-func.c \
	preferences/gy-preferences-file-chooser.c \
	preferences/gy-preferences-page.c \
	preferences/gy-preferences-page-dicts.c \
	preferences/gy-preferences-page-general.c \
	preferences/gy-preferences-switch.c \
	preferences/gy-preferences-window.c \
	printing/gy-print.c \
	printing/gy-print-compositor.c \
	search/gy-search-bar.c \
	shortcuts/gy-shortcuts-window.c \
	window/gy-header-bar.c \
	window/gy-window-settings.c \
	window/gy-window.c \
	window/gy-workspace.c \
	$(NULL)

nodist_libgydict_la_SOURCES = \
	resources/gy-resources.h \
	resources/gy-resources.c \
	$(NULL)

libgydict_la_CPPFLAGS = \
	-I$(srcdir) \
	-I$(top_builddir)/contrib/pnl \
	-I$(top_srcdir)/contrib/pnl \
	-I$(top_builddir)/contrib/egg \
	-I$(top_srcdir)/contrib/egg \
	$(CPPFLAGS) \
	$(NULL)

libgydict_la_CFLAGS = \
	$(CFLAGS) \
	$(GYDICT_CFLAGS)
	$(NULL)

libgydict_la_LIBADD = \
	$(LIBM) \
	$(GYDICT_LIBS) \
	$(top_builddir)/contrib/egg/libegg-private.la \
	$(top_builddir)/contrib/pnl/libpanel-gtk.la \
	$(NULL)

libgydict_la_LDFLAGS = \
	$(LDFLAGS) \
	$(NULL)

glib_resources_h = resources/gy-resources.h
glib_resources_c = resources/gy-resources.c
glib_resources_xml = resources/gy.gresource.xml
glib_resources_namespace = gy
include $(top_srcdir)/tools/autotools/Makefile.am.gresources


-include $(top_srcdir)/git.mk
