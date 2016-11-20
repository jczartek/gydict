CLEANFILES =
DISTCLEANFILES =
BUILT_SOURCES =
EXTRA_DIST =

gydict_common_cppflags =                                               \
	-I$(srcdir)                                                    \
	-I$(top_srcdir)/libgydict                                      \
	-DPACKAGE_DATA_DIR=\""$(datadir)"\"                            \
	-DPACKAGE_LOCAL_DIR=\""$(datadir)/locale"\"                    \
        $(CPPFLAGS)                                                    \
        $(NULL)

gydict_common_cflags =                                                 \
	$(CFLAGS)                                                      \
	$(GYDICT_CFLAGS)                                               \
	$(NULL)

gydict_common_ldflags =                                                \
	-export-dynamic                                                \
	$(LDFLAGS)                                                     \
	$(NULL)

bin_PROGRAMS = gydict

gydict_SOURCES =                                                       \
	gy-main.c                                                      \
	$(null)

gydict_CPPFLAGS =                                                      \
	$(gydict_common_cppflags)                                      \
	$(NULL)

gydict_CFLAGS =                                                        \
	$(gydict_common_cflags)                                        \
	$(NULL)

gydict_LDFLAGS =                                                       \
	$(gydict_common_ldflags)                                       \
	$(NULL)

gydict_LDADD =                                                         \
	$(top_srcdir)/libgydict/libgydict.la                           \
	$(LIBM)                                                        \
	$(GYDICT_LIBS)                                                 \
	$(NULL)

-include $(top_srcdir)/git.mk
