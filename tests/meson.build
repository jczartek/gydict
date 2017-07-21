
test_cflags = [
  '-DTEST_DATA_DIR="@0@/"'.format(meson.current_source_dir())
]

test_panel_gtk = executable('test-panel-gtk', 'test-panel-gtk.c',
        c_args: test_cflags,
  dependencies: [pnl_dep, gtk_dep])

test('test-panel-gtk', test_panel_gtk)

test_dict_manager = executable('test-dict-manager', 'test-dict-manager.c',
        c_args: test_cflags,
  dependencies: [libgydict_dep])

test('test-dict-manager', test_dict_manager)
