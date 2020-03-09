app_headers = [
  'gy-app.h',
  'gy-app-addin.h'
]

app_sources = [
  'gy-app.c',
  'gy-app-addin.c'
]

app_private_headers = [
  'gy-app-private.h'
]

app_private_sources = [
  'gy-app-actions.c',
  'gy-app-shortcuts.c',
  'gy-app-cmd-line.c',
]

libgydict_public_headers  += files(app_headers)
libgydict_public_sources  += files(app_sources)
libgydict_private_headers += files(app_private_headers)
libgydict_private_sources += files(app_private_sources)

install_headers(app_headers, install_dir: join_paths(libgydict_header_dir, 'app'))
