resource_files = $(shell glib-compile-resources --sourcedir=$(srcdir) --generate-dependencies $(srcdir)/gy.gresource.xml)
gy-resources.c: gy.gresource.xml $(resource_files)
	glib-compile-resources --target=$@ --sourcedir=$(srcdir) --generate-source --c-name gy $(srcdir)/gy.gresource.xml
gy-resources.h: gy.gresource.xml $(resource_files)
	glib-compile-resources --target=$@ --sourcedir=$(srcdir) --generate-header --c-name gy $(srcdir)/gy.gresource.xml

gydict_common_cppflags =                                                       \
		-I$(srcdir)                                                    \
		-I$(srcdir)/src                                                \
		-DPACKAGE_DATA_DIR=\""$(datadir)"\"                            \
		-DPACKAGE_LOCAL_DIR=\""$(datadir)/locale"\"                    \
		$(CPPFLAGS)                                                    \
		$(NULL)

gydict_common_cflags =                                                         \
		$(CFLAGS)                                                      \
		$(GYDICT_CFLAGS)                                               \
		-pipe                                                          \
		-std=c99                                                       \
		$(NULL)

gydict_common_ldflags =                                                        \
	  	-export-dynamic                                                \
		$(LDFLAGS)                                                     \
		$(NULL)

BUILT_SOURCES =                                                                \
		gy-resources.c                                                 \
		gy-resources.h                                                 \
		$(NULL)

EXTRA_DIST =                                                                   \
		$(srcdir)/resources/ui/gydict.ui                               \
		$(srcdir)/resources/ui/menu.ui	                               \
		$(resource_files)                                              \
		$(NULL)

bin_PROGRAMS = gydict

gydict_SOURCES =                                                               \
		gy-app.c                                                       \
		gy-depl.c                                                      \
		gy-main.c                                                      \
		gy-dialog-pref.c                                               \
		gy-window.c                                                    \
		gy-utility-func.c                                              \
		gy-dict.c                                                      \
		gy-parser.c                                                    \
		gy-parser-pwn.c                                                \
		gy-parser-pwn.h                                                \
                gy-print.c                                                     \
		gy-history.c                                                   \
                gy-history-iterable.c                                          \
		gy-search-bar.c                                                \
		gy-settings.c                                                  \
		gy-print-compositor.c                                          \
		gy-resources.c                                                 \
		gy-pwn.c                                                       \
		gy-enum-types.c                                                \
		gy-pwntabs.c                                                   \
		$(NULL)

gydict_CPPFLAGS =                                                              \
		$(gydict_common_cppflags)                                      \
		$(NULL)

gydict_CFLAGS =                                                                \
		$(gydict_common_cflags)                                        \
		$(NULL)

gydict_LDFLAGS =                                                               \
		$(gydict_common_ldflags)                                       \
		$(NULL)

gydict_LDADD =                                                                 \
		-lz                                                            \
		$(LIBM)                                                        \
		$(GYDICT_LIBS)                                                 \
		$(NULL)
