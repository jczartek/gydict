preferences_headers = [
  'gy-preferences-file-chooser.h',
  'gy-preferences-page.h',
  'gy-preferences-page-dicts.h',
  'gy-preferences-page-general.h',
  'gy-preferences-switch.h',
  'gy-preferences-window.h'
]

preferences_sources = [
  'gy-preferences-file-chooser.c',
  'gy-preferences-page.c',
  'gy-preferences-page-dicts.c',
  'gy-preferences-page-general.c',
  'gy-preferences-switch.c',
  'gy-preferences-window.c'
]

libgydict_public_headers += files(preferences_headers)
libgydict_public_sources += files(preferences_sources)

install_headers(preferences_headers, install_dir: join_paths(libgydict_header_dir, 'preferences'))
