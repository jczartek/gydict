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

TESTS = test-hello
test_hello_SOURCES = test-hello.c
test_hello_CFLAGS = $(tests_cflags)
test_hello_LDADD = $(tests_libs)
test_hello_LDFLAGS = $(tests_ldflags)

check_PROGRAMS = $(TESTS)
