printing_headers = [
  'gy-print.h',
  'gy-print-compositor.h'
]

printing_sources = [
  'gy-print.c',
  'gy-print-compositor.c'
]

libgydict_public_headers += files(printing_headers)
libgydict_public_sources += files(printing_sources)

install_headers(printing_headers, install_dir: join_paths(libgydict_header_dir, 'printing'))
