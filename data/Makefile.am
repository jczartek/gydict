gsettings_SCHEMAS = \
	org.gtk.gydict.gschema.xml \
	org.gtk.gydict.workspace.gschema.xml \
	$(NULL)

@GSETTINGS_RULES@

desktopdir = $(datadir)/applications
desktop_in_in_files = gydict.desktop.in.in
desktop_in_files = gydict.desktop.in
desktop_DATA = $(desktop_in_files:.desktop.in=.desktop)

%.desktop: %.desktop.in
	$(AM_V_GEN)$(MSGFMT) --desktop --template $< -d $(top_srcdir)/po -o $@


# pkg config
pkgconfigdir = $(libdir)/gydict/pkgconfig
pkgconfig_DATA = \
	libgydict.pc \
	$(NULL)

EXTRA_DIST = 			\
	$(gsettings_SCHEMAS)    \
	$(desktop_in_in_files)  \
	$(NULL)

DISTCLEANFILES =		\
	$(desktop_in_files)     \
	$(NULL)

CLEANFILES =                    \
	$(desktop_DATA)         \
	$(desktop_in_files)     \
	$(NULL)

-include $(top_srcdir)/git.mk
