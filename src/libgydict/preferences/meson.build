preferences_headers = [
  'gy-prefs-view.h',
  'gy-prefs-window.h'
]

preferences_sources = [
  'gy-prefs-view.c',
  'gy-prefs-window.c'
]

libgydict_public_headers += files(preferences_headers)
libgydict_public_sources += files(preferences_sources)

install_headers(preferences_headers, install_dir: join_paths(libgydict_header_dir, 'preferences'))
