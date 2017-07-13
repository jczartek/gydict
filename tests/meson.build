
test_cflags = [
  '-DTEST_DATA_DIR="@0@/"'.format(meson.current_source_dir())
]

test_panel_gtk = executable('test-panel-gtk', 'test-panel-gtk.c',
        c_args: test_cflags,
  dependencies: [pnl_dep, gtk_dep])

test('test-panel-gtk', test_panel_gtk)
