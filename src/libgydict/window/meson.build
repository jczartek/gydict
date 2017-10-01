window_headers = [
  'gy-header-bar.h',
  'gy-store-entry.h',
  'gy-window.h',
  'gy-workspace.h'
]

window_sources = [
  'gy-header-bar.c',
  'gy-store-entry.c',
  'gy-window.c',
  'gy-workspace.c'
]

window_private = [
  'gy-window-settings.h',
  'gy-window-settings.c'
]

libgydict_public_headers   += files(window_headers)
libgydict_public_sources   += files(window_sources)
libgydict_private_sources  += files(window_private)

install_headers(window_headers, install_dir: join_paths(libgydict_header_dir, 'window'))
