ACLOCAL_AMFLAGS = -I m4 ${ACLOCAL_FLAGS}

SUBDIRS = data icons po src

gydictdocdir = ${prefix}/doc/gydict
gydictdoc_DATA = \
		 README \
		 COPYING \
		 AUTHORS \
		 ChangeLog \
		 INSTALL \
		 NEWS

# Remove doc directory on uninstall
uninstall-local:
	rm -r ${gydictdocdir}

