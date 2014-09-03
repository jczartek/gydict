
AC_INIT([gydict], [0.1.0])

AC_CONFIG_SRCDIR([src/gy-main.c])
AC_CONFIG_HEADERS([src/config.h])
AC_CONFIG_MACRO_DIR([m4])
AC_CONFIG_AUX_DIR([build-aux])

AM_INIT_AUTOMAKE([1.11])

AM_SILENT_RULES([yes])

AC_PROG_CC
AC_PROG_CPP
AM_PROG_CC_C_O

AC_HEADER_STDC

LT_PREREQ([2.2])
LT_INIT([disable-static])

GLIB_GSETTINGS

dnl ###############################################################################
dnl Internationalization
dnl ###############################################################################
IT_PROG_INTLTOOL(0.50.2)
AM_GNU_GETTEXT_VERSION([0.17])
AM_GNU_GETTEXT([external])
GETTEXT_PACKAGE=gydict
AC_SUBST(GETTEXT_PACKAGE)
AC_DEFINE_UNQUOTED(GETTEXT_PACKAGE,"$GETTEXT_PACKAGE",[Package name for gettext])
dnl ###############################################################################

dnl ###############################################################################
dnl Dependencies
dnl ###############################################################################

PKG_PROG_PKG_CONFIG

AC_CHECK_LIB(z,uncompress,
[],
AC_MSG_ERROR(Function uncompress not found!!!)
)

AC_CHECK_LIBM
AC_SUBST(LIBM)

PKG_CHECK_MODULES(GYDICT, [
  gtk+-3.0 >= 3.10.0
  gio-2.0 >= 2.28.0
  glib-2.0 >= 2.38.0
])
dnl ################################################################################

AC_OUTPUT([
Makefile
src/Makefile
data/Makefile
data/gydict.desktop.in
icons/Makefile
po/Makefile.in
])

echo
echo "Prefix: 		$prefix"
echo
