AC_PREREQ(2.63.2)

dnl ****************************************************************************
dnl * Define Versioning Information                                            *
dnl ****************************************************************************
m4_define(gydict_major_version, 0)
m4_define(gydict_minor_version, 3)
m4_define(gydict_micro_version, 0)
m4_define(gydict_version, gydict_major_version.gydict_minor_version.gydict_micro_version)

AC_DEFINE(GYDICT_MAJOR_VERSION, gydict_major_version, [Gydict major version])
AC_SUBST(GYDICT_MAJOR_VERSION, gydict_major_version)
AC_DEFINE(GYDICT_MINOR_VERSION, gydict_minor_version, [Gydict minor version])
AC_SUBST(GYDICT_MINOR_VERSION, gydict_minor_version)
AC_DEFINE(GYDICT_MICRO_VERSION, gydict_micro_version, [Gydict micro version])
AC_SUBST(GYDICT_MICRO_VERSION, gydict_micro_version)

dnl ****************************************************************************
dnl * Initialize Autoconf                                                      *
dnl ****************************************************************************
AC_INIT([gydict],
        [gydict_version],
        [],
        [gydict],
        [https://github.com/jczartek])
AC_CONFIG_SRCDIR([src/gy-main.c])
AC_CONFIG_HEADERS([src/config.h])
AC_CONFIG_MACRO_DIR([m4])
AC_CONFIG_AUX_DIR([build-aux])

dnl ****************************************************************************
dnl * Initialize Automake                                                      *
dnl ****************************************************************************
AM_INIT_AUTOMAKE([1.11 tar-ustar dist-xz no-dist-gzip -Wall])
AM_SILENT_RULES([yes])

dnl ****************************************************************************
dnl * Check for Required Programs                                              *
dnl ****************************************************************************
AC_PROG_CC
AC_PROG_CPP
AM_PROG_CC_C_O
AC_PROG_INSTALL
AC_PROG_MAKE_SET
PKG_PROG_PKG_CONFIG
AC_HEADER_STDC
AC_PATH_PROG(GLIB_COMPILE_RESOURCES, glib-compile-resources)
GLIB_GSETTINGS

dnl ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
dnl Initialize Libtool
dnl ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
LT_PREREQ([2.2])
LT_INIT([disable-shared])

dnl ****************************************************************************
dnl * Internationalization                                                     *
dnl ****************************************************************************
GETTEXT_PACKAGE=gydict
AC_SUBST(GETTEXT_PACKAGE)
AC_DEFINE_UNQUOTED(GETTEXT_PACKAGE,"$GETTEXT_PACKAGE",[Package name for gettext])
AM_GNU_GETTEXT_VERSION([0.19.8])
AM_GNU_GETTEXT([external])

dnl ****************************************************************************
dnl * Check for Required Packages                                              *
dnl ****************************************************************************
m4_define([gtk_required_version], [3.20.0])
m4_define([glib_required_version], [2.38.0])
m4_define([zlib_required_version], [1.2.8])

PKG_CHECK_MODULES(GYDICT,    [gtk+-3.0 >= gtk_required_version
                              gio-2.0 >= glib_required_version
                              glib-2.0 >= glib_required_version
                              zlib >= zlib_required_version])
PKG_CHECK_MODULES(PANEL_GTK, [gtk+-3.0 >= gtk_required_version])

AC_CHECK_LIBM
AC_SUBST(LIBM)

dnl ****************************************************************************
dnl * Check if shm_open Requires a Library                                     *
dnl ****************************************************************************
AC_CHECK_FUNCS([shm_open], [SHM_LIB=],
               [AC_CHECK_LIB([rt], [shm_open], [SHM_LIB=-lrt], [SHM_LIB=])])
AC_SUBST(SHM_LIB)

dnl ****************************************************************************
dnl * Always build tests                                                       *
dnl ****************************************************************************
AC_ARG_ENABLE([tests],
              AS_HELP_STRING([--enable-tests],
                             [Enable always building tests during 'make all' [default=no]]),
              [enable_tests=$enableval], [enable_tests=no])

AM_CONDITIONAL([ENABLE_TESTS], test "$enable_tests" = "yes")


dnl ***********************************************************************
dnl Check if we should instrument our targets
dnl ***********************************************************************
AC_MSG_CHECKING([for --enable-profiling])
AC_ARG_ENABLE(profiling,
              AC_HELP_STRING([--enable-profiling],
                             [turn on runtime instrumentation [default=no]]),
              [enable_profiling=$enableval],
	      [enable_profiling=no])
AS_IF([test "$enable_profiling" = "yes"],[
	ac_save_cflags="$CFLAGS"
	CFLAGS="$CFLAGS -pg"
	AC_LINK_IFELSE([AC_LANG_SOURCE([[int main() { return 0; }]])],
	               [PROFILING_CFLAGS="-pg"],
		       [enable_profiling=no])
	CFLAGS="$ac_save_cflags $PROFILING_CFLAGS"
])
AC_MSG_RESULT([$enable_profiling])

AC_OUTPUT([
Makefile
contrib/Makefile
contrib/pnl/Makefile
contrib/pnl/pnl-version.h
data/Makefile
data/gydict.desktop.in
icons/Makefile
po/Makefile.in
src/Makefile
tests/Makefile
])

echo ""
echo " ${PACKAGE} - ${VERSION}"
echo ""
echo " Options"
echo ""
echo "  Prefix .............................. : ${prefix}"
echo "  Libdir .............................. : ${libdir}"
echo ""
echo " Development Options"
echo ""
echo "  Enable Profiling (-pg)............... : ${enable_profiling}"
echo "  Build Test Suite .................... : ${enable_tests}"
echo ""

