#!/bin/sh


#aclocal && automake && autoconf

#if [ $# = 0 ]
#then
#	echo "Generate files configure and makefiles"
#	autoheader && aclocal && automake && autoconf
#elif [ $1 = '--clean' ]
#then
#	echo "Remove all makefiles"
#	make distclean
#	rm */Makefile.in
#	rm ./configure
#	rm src/config.h.in
#else
#	echo "Nieprawidłowy parametr. Użyj ./autogen lub ./autogen --clean"
#fi

srcdir=`dirname $0`
test -z "$srcdir" && srcdir=.

(test -f $srcdir/src/gy-main.c) || {
  echo -n "**Error**: Directory "\`$srcdir\`" does not look the"
  echo "top-level Gydict directory."
  exit 1
}

which gnome-autogen.sh || {
  echo "You need to install gnome-common."
  exit 1
}

. gnome-autogen.sh
