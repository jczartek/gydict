SUBDIRS = contrib

resource_files = $(shell glib-compile-resources --sourcedir=$(srcdir) --generate-dependencies $(srcdir)/gy.gresource.xml)
gy-resources.c: gy.gresource.xml $(resource_files)
	glib-compile-resources --target=$@ --sourcedir=$(srcdir) --generate-source --c-name gy $(srcdir)/gy.gresource.xml
gy-resources.h: gy.gresource.xml $(resource_files)
	glib-compile-resources --target=$@ --sourcedir=$(srcdir) --generate-header --c-name gy $(srcdir)/gy.gresource.xml

gydict_common_cppflags =                                                       \
                -I$(srcdir)                                                    \
                -I$(srcdir)/contrib/shortcuts                                  \
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

gydict_common_libs =                                                           \
               $(srcdir)/contrib/shortcuts/libshortcuts-private.la             \
               $(NULL)

BUILT_SOURCES =                                                                \
		gy-resources.c                                                 \
		gy-resources.h                                                 \
		$(NULL)

EXTRA_DIST =                                                                   \
		$(srcdir)/resources/ui/gydict.ui                               \
		$(srcdir)/resources/ui/menu.ui	                               \
		$(resource_files)                                              \
                $(srcdir)/gy.gresource.xml                                     \
		$(NULL)

bin_PROGRAMS = gydict

gydict_SOURCES =                                                               \
		gy-app.c gy-app.h                                              \
		gy-depl.c gy-depl.h                                            \
		gy-main.c                                                      \
		gy-window.c gy-window.h                                        \
		gy-utility-func.c gy-utility-func.h                            \
		gy-dict.c gy-dict.h                                            \
                gy-parsable.c gy-parsable.h                                    \
		gy-parser-pwn.c                                                \
		gy-parser-pwn.h                                                \
                gy-print.c gy-print.h                                          \
		gy-history.c gy-history.h                                      \
                gy-history-iterable.c gy-history-iterable.h                    \
		gy-search-bar.c gy-search-bar.h                                \
		gy-print-compositor.c gy-print-compositor.h                    \
		gy-resources.c gy-resources.h                                  \
		gy-pwn.c gy-pwn.h                                              \
		gy-enum-types.c gy-enum-types.h                                \
		gy-pwntabs.c gy-pwntabs.h                                      \
                gy-preferences-window.c gy-preferences-window.h                \
                gy-preferences-page.c gy-preferences-page.h                    \
                gy-preferences-page-general.c gy-preferences-page-general.h    \
                gy-preferences-switch.c gy-preferences-switch.h                \
                gy-text-view.c gy-text-view.h                                  \
                gy-preferences-page-dicts.c gy-preferences-page-dicts.h        \
                gy-preferences-file-chooser.c gy-preferences-file-chooser.h    \
                gy-css-provider.c gy-css-provider.h                            \
                gy-german-pwn.c gy-german-pwn.h                                \
                gy-english-pwn.c gy-english-pwn.h                              \
                gy-pwn-dict.c gy-pwn-dict.h                                    \
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
		$(gydict_common_libs)
