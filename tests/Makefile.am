CLEANFILES =
DISTCLEANFILES =
BUILT_SOURCES =
EXTRA_DIST =

tests_cppflags = \
	-I$(top_builddir)/libgydict \
	-I$(top_srcdir)/libgydict \
	$(NULL)

tests_cflags = \
	$(tests_cppflags) \
	$(GYDICT_CFLAGS) \
	$(NULL)

tests_libs = \
	$(GYDICT_LIBS) \
	$(top_srcdir)/libgydict/libgydict.la \
	$(NULL)

tests_ldflags = \
	-export-dynamic \
	$(NULL)

tests_panel_gtk_cflags = \
	$(PANEL_GTK_CFLAGS) \
	-I$(top_srcdir)/contrib/pnl \
	$(NULL)

tests_panel_gtk_libs = \
	$(PANEL_GTK_LIBS) \
	$(top_srcdir)/contrib/pnl/libpanel-gtk.la \
	$(NULL)


TESTS = test-hello
test_hello_SOURCES = test-hello.c
test_hello_CFLAGS = $(tests_cflags)
test_hello_LDADD = $(tests_libs)
test_hello_LDFLAGS = $(tests_ldflags)

TESTS += test-dict-manager
test_dict_manager_SOURCES = test-dict-manager.c
test_dict_manager_CFLAGS = $(tests_cflags)
test_dict_manager_LDADD = $(tests_libs)
test_dict_manager_LDFLAGS = $(tests_ldflags)

#TESTS += test-panel-gtk
#test_panel_gtk_SOURCES = test-panel-gtk.c
#test_panel_gtk_CFLAGS = $(tests_panel_gtk_cflags)
#test_panel_gtk_LDADD = $(tests_panel_gtk_libs)
#test_panel_gtk_LDFLAGS = $(tests_ldflags)
EXTRA_DIST += \
	test-panel-gtk.ui \
	$(NULL)

#TESTS += test-header-bar
#test_header_bar_SOURCES = test-header-bar.c
#test_header_bar_CFLAGS = $(tests_cflags)
#test_header_bar_LDADD = $(tests_libs)
#test_header_bar_LDFLAGS = $(tests_ldflags)

TESTS += test-history
test_history_SOURCES = test-history.c
test_history_CFLAGS = $(tests_cflags)
test_history_LDADD = $(tests_libs)
test_history_LDFLAGS = $(tests_ldflags)

if ENABLE_TESTS
noinst_PROGRAMS = $(TESTS)
endif

check_PROGRAMS = $(TESTS)

-include $(top_srcdir)/git.mk
