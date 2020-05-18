window_headers = [
  'gy-window-addin.h',
  'gy-search-bar.h',
  'gy-def-list.h',
  'gy-text-buffer.h',
  'gy-text-view.h',
  'gy-window.h',
]

window_sources = [
  'gy-window-addin.c',
  'gy-search-bar.c',
  'gy-def-list.c',
  'gy-text-buffer.c',
  'gy-text-view.c',
  'gy-window.c',
]

window_private = [
  'gy-window-private.h',
  'gy-window-settings.h',
  'gy-window-settings.c',
  'gy-window-plugins.c',
  'gy-window-actions.c',
]

libgydict_public_headers   += files(window_headers)
libgydict_public_sources   += files(window_sources)
libgydict_private_sources  += files(window_private)

install_headers(window_headers, install_dir: join_paths(libgydict_header_dir, 'gui'))
