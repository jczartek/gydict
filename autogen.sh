#!/bin/sh

srcdir=`dirname $0`
test -z "$srcdir" && srcdir=.

(test -f $srcdir/src/gy-main.c) || {
  echo -n "**Error**: Directory "\`$srcdir\`" does not look the"
  echo "top-level Gydict directory."
  exit 1
}

autoreconf --force --install -Wno-portability || exit 1
