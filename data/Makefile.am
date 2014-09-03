#

gsettings_SCHEMAS = org.gtk.gydict.gschema.xml
@GSETTINGS_RULES@

@INTLTOOL_DESKTOP_RULE@

desktopdir = $(datadir)/applications
desktop_in_files = gydict.desktop.in
desktop_DATA = $(desktop_in_files:.desktop.in=.desktop)

EXTRA_DIST = 			\
	$(gsettings_SCHEMAS) \
	$(desktop_in_file)

DISTCLEANFILES =		\
	$(desktop_DATA)
