EXTRA_DIST =

tests_cflags = \
	$(GYDICT_CFLAGS)
	$(NULL)

tests_libs = \
	$(GYDICT_LIBS)
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

TESTS += test-panel-gtk
test_panel_gtk_SOURCES = test-panel-gtk.c
test_panel_gtk_CFLAGS = $(tests_panel_gtk_cflags)
test_panel_gtk_LDADD = $(tests_panel_gtk_libs)
test_panel_gtk_LDFLAGS = $(tests_ldflags)
EXTRA_DIST += \
	test-panel-gtk.ui

if ENABLE_TESTS
noinst_PROGRAMS = $(TESTS)
endif

check_PROGRAMS = $(TESTS)
