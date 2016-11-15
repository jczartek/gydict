CLEANFILES =
DISTCLEANFILES =
EXTRA_DIST =
BUILT_SOURCES =

pkglibdir = $(libdir)/gydict
pkglib_LTLIBRARIES = libpanel-gtk.la

headersdir = $(includedir)/gydict/pnl
headers_DATA = \
	pnl-animation.h \
	pnl-dock-bin.h \
	pnl-dock-bin-edge.h \
	pnl-dock-item.h \
	pnl-dock-manager.h \
	pnl-dock-overlay.h \
	pnl-dock-paned.h \
	pnl-dock-revealer.h \
	pnl-dock-stack.h \
	pnl-dock-tab-strip.h \
	pnl-dock-transient-grab.h \
	pnl-dock-types.h \
	pnl-dock-widget.h \
	pnl-dock-window.h \
	pnl-dock.h \
	pnl-frame-source.h \
	pnl-multi-paned.h \
	pnl-tab-strip.h \
	pnl-tab.h \
	pnl-version.h \
	pnl.h \
	$(NULL)

libpanel_gtk_la_SOURCES = \
	$(headers_DATA) \
	pnl-animation.c \
	pnl-dock-bin-edge.c \
	pnl-dock-bin-edge-private.h \
	pnl-dock-bin.c \
	pnl-dock-item.c \
	pnl-dock-manager.c \
	pnl-dock-overlay-edge-private.h \
	pnl-dock-overlay-edge.c \
	pnl-dock-overlay.c \
	pnl-dock-paned-private.h \
	pnl-dock-paned.c \
	pnl-dock-revealer.c \
	pnl-dock-stack.c \
	pnl-dock-tab-strip.c \
	pnl-dock-transient-grab.c \
	pnl-dock-widget.c \
	pnl-dock-window.c \
	pnl-dock.c \
	pnl-frame-source.c \
	pnl-multi-paned.c \
	pnl-tab-strip.c \
	pnl-tab.c \
	pnl-util-private.h \
	pnl-util.c \
	$(NULL)

nodist_libpanel_gtk_la_SOURCES = pnl-resources.c pnl-resources.h

libpanel_gtk_la_CFLAGS = \
	-DPNL_COMPILATION \
	$(PANEL_GTK_CFLAGS) \
	$(NULL)

libpanel_gtk_la_LIBADD = \
	$(PANEL_GTK_LIBS) \
	$(NULL)

libpanel_gtk_la_LDFLAGS = \
	-no-undefined \
	$(NULL)


glib_resources_h = pnl-resources.h
glib_resources_c = pnl-resources.c
glib_resources_xml = pnl.gresource.xml
glib_resources_namespace = pnl
include $(top_srcdir)/tools/autotools/Makefile.am.gresources

