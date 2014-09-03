resource_files = $(shell glib-compile-resources --sourcedir=$(srcdir) --generate-dependencies $(srcdir)/gy.gresource.xml)
gy-resources.c: gy.gresource.xml $(resource_files)
	glib-compile-resources --target=$@ --sourcedir=$(srcdir) --generate-source --c-name gy $(srcdir)/gy.gresource.xml
gy-resources.h: gy.gresource.xml $(resource_files)
	glib-compile-resources --target=$@ --sourcedir=$(srcdir) --generate-header --c-name gy $(srcdir)/gy.gresource.xml

BUILT_SOURCES =         \
	gy-resources.c  \
	gy-resources.h  \
	$(NULL)
                        
EXTRA_DIST = 			\
	gydict.ui		\
	menu.ui			\
	$(resource_files)       \
	$(NULL)

AM_CPPFLAGS = \
	      -DPACKAGE_DATA_DIR=\""${pkgdatadir}"\" \
	      -DPACKAGE_LOCAL_DIR=\""$(datadir)/locale"\" \
	      ${CPPFLAGS}

AM_CFLAGS = \
	    -pipe \
	    -std=c99 \
	    ${CFLAGS}

AM_LDFLAGS = \
	     ${LDFLAGS}

bin_PROGRAMS = gydict

gydict_SOURCES = \
		 gy-app.c		\
		 gy-depl.c		\
		 gy-main.c		\
		 gy-dialog-pref.c	\
		 gy-window.c		\
		 gy-utility-func.c	\
		 gy-dict.c		\
		 gy-parser.c		\
		 gy-print.c		\
		 gy-history.c		\
		 gy-search-bar.c	\
		 gy-settings.c		\
		 gy-print-compositor.c	\
		 gy-resources.c		\
		 gy-pwn.c		\
		 gy-enum-types.c	\
		 gy-pwntabs.c

gydict_CPPFLAGS = \
		$(AM_CPPFLAGS)

gydict_CFLAGS = \
		$(AM_CFLAGS) \
		$(GYDICT_CFLAGS)

gydict_LDADD = \
	       -lz \
	       $(LIBM) \
	       $(GYDICT_LIBS)
