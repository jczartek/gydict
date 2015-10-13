#!/bin/sh


#aclocal && automake && autoconf

if [ $# = 0 ]
then
	echo "Generate files configure and makefiles"
	autoheader && aclocal && automake && autoconf
elif [ $1 = '--clean' ]
then
	echo "Remove all makefiles"
	make distclean
	rm */Makefile.in
	rm ./configure
	rm src/config.h.in
else
	echo "Nieprawidłowy parametr. Użyj ./autogen lub ./autogen --clean"
fi
