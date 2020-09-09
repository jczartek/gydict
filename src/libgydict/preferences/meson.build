preferences_headers = [
  'gy-prefs-view.h',
  'gy-prefs-view-addin.h',
  'gy-prefs-window.h',
  'gy-prefs-configurable.h',
  'gy-prefs-plugin-row.h',
]

preferences_sources = [
  'gy-prefs-view.c',
  'gy-prefs-view-addin.c',
  'gy-prefs-window.c',
  'gy-prefs-configurable.c',
  'gy-prefs-plugin-row.c',
]

libgydict_public_headers += files(preferences_headers)
libgydict_public_sources += files(preferences_sources)

install_headers(preferences_headers, install_dir: join_paths(libgydict_header_dir, 'preferences'))
