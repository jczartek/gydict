window_headers = [
  'gy-header-bar.h',
  'gy-history-box.h',
  'gy-window.h',
]

window_sources = [
  'gy-header-bar.c',
  'gy-history-box.c',
  'gy-window.c',
]

window_private = [
  'gy-window-settings.h',
  'gy-window-settings.c'
]

libgydict_public_headers   += files(window_headers)
libgydict_public_sources   += files(window_sources)
libgydict_private_sources  += files(window_private)

install_headers(window_headers, install_dir: join_paths(libgydict_header_dir, 'window'))
