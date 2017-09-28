entryview_headers = [
  'gy-text-buffer.h',
  'gy-text-view.h'
]

entryview_sources = [
  'gy-text-buffer.c',
  'gy-text-view.c'
]

entryview_private = [
  'gy-text-buffer-private.h'
]

libgydict_public_headers  += files(entryview_headers)
libgydict_public_sources  += files(entryview_sources)
libgydict_private_sources += files(entryview_private)

install_headers(entryview_headers, install_dir: join_paths(libgydict_header_dir, 'entryview'))
