gsettings_SCHEMAS = org.gtk.gydict.gschema.xml
@GSETTINGS_RULES@

desktopdir = $(datadir)/applications
desktop_in_in_files = gydict.desktop.in.in
desktop_in_files = gydict.desktop.in
desktop_DATA = $(desktop_in_files:.desktop.in=.desktop)

%.desktop: %.desktop.in
	$(AM_V_GEN)$(MSGFMT) --desktop --template $< -d $(top_srcdir)/po -o $@


EXTRA_DIST = 			\
	$(gsettings_SCHEMAS)
#	$(desktop_in_file)

DISTCLEANFILES =		\
	$(desktop_DATA)
