AC_PREREQ(2.63.2)

m4_define(gydict_major_version, 0)
m4_define(gydict_minor_version, 3)
m4_define(gydict_micro_version, 0)
m4_define(gydict_version, gydict_major_version.gydict_minor_version.gydict_micro_version)

AC_INIT([gydict],
        [gydict_version],
        [],
        [gydict],
        [https://github.com/jczartek])

dnl ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
dnl + Dependencies                                                             +
dnl ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

GLIB_REQUIRED=2.46.0
GTK_REQUIRED=3.18.0

AC_CONFIG_SRCDIR([src/gy-main.c])
AC_CONFIG_HEADERS([src/config.h])
AC_CONFIG_MACRO_DIR([m4])
AC_CONFIG_AUX_DIR([build-aux])

AC_DEFINE(GYDICT_MAJOR_VERSION, gydict_major_version, [Gydict major version])
AC_SUBST(GYDICT_MAJOR_VERSION, gydict_major_version)
AC_DEFINE(GYDICT_MINOR_VERSION, gydict_minor_version, [Gydict minor version])
AC_SUBST(GYDICT_MINOR_VERSION, gydict_minor_version)
AC_DEFINE(GYDICT_MICRO_VERSION, gydict_micro_version, [Gydict micro version])
AC_SUBST(GYDICT_MICRO_VERSION, gydict_micro_version)

AM_INIT_AUTOMAKE([1.11 tar-ustar dist-xz no-dist-gzip -Wall])

AM_SILENT_RULES([yes])

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
dnl + Internationalization                                                     +
dnl ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
IT_PROG_INTLTOOL(0.50.2)
GETTEXT_PACKAGE=gydict
AC_SUBST(GETTEXT_PACKAGE)
AC_DEFINE_UNQUOTED(GETTEXT_PACKAGE,"$GETTEXT_PACKAGE",[Package name for gettext])


AC_CHECK_LIB(z,uncompress,
[],
AC_MSG_ERROR(Function uncompress not found!!!)
)

AC_CHECK_LIBM
AC_SUBST(LIBM)

PKG_CHECK_MODULES(GYDICT, [
  gtk+-3.0 >= $GTK_REQUIRED
  gio-2.0 >= $GLIB_REQUIRED
  glib-2.0 >= $GLIB_REQUIRED
])

AC_SUBST(GYDICT_CFLAGS)
AC_SUBST(GYDICT_LIBS)

AC_CHECK_FUNCS([shm_open], [SHM_LIB=],
               [AC_CHECK_LIB([rt], [shm_open], [SHM_LIB=-lrt], [SHM_LIB=])])
AC_SUBST(SHM_LIB)

AC_OUTPUT([
Makefile
src/Makefile
data/Makefile
data/gydict.desktop.in
icons/Makefile
po/Makefile.in
])

echo "
     Prefix............................... ${prefix}
   Compiler............................... ${CC}
     CFlags............................... ${CFLAGS}
GydictFlags............................... ${GYDICT_CFLAGS}
       Libs............................... ${LIBS}
 GydictLibs............................... ${GYDICT_LIBS}"

