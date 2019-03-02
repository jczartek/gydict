icondir = join_paths(gydict_datadir, 'icons/hicolor')

install_subdir('icons/16x16', install_dir: icondir)
install_subdir('icons/22x22', install_dir: icondir)
install_subdir('icons/24x24', install_dir: icondir)
install_subdir('icons/32x32', install_dir: icondir)
install_subdir('icons/48x48', install_dir: icondir)
install_subdir('icons/256x256', install_dir: icondir)

desktop_file = i18n.merge_file(
  input: 'org.gtk.gydict.in.in',
  output: 'org.gtk.gydict.desktop',
  type: 'desktop',
  po_dir: po_dir,
  install: true,
  install_dir: join_paths(gydict_datadir, 'applications')
)

desktop_utils = find_program('desktop-file-validate', required: false)
if desktop_utils.found()
  test('Validate desktop file', desktop_utils, args: [desktop_file])
endif

install_data([
  'org.gtk.gydict.gschema.xml',
], install_dir: gydict_schemadir)

# D-Bus service file.
dbusconf = configuration_data()
dbusconf.set('bindir', join_paths(get_option('prefix'), get_option('bindir')))
configure_file(
	input: 'org.gtk.gydict.service.in',
	output: 'org.gtk.gydict.service',
	configuration: dbusconf,
	install: true,
	install_dir: join_paths(get_option('datadir'), 'dbus-1', 'services'),
)
