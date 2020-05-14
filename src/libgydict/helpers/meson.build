helpers_headers = [
  'gy-format-scheme.h',
  'gy-text-attribute.h',
  'gy-utility-func.h'
]

helpers_sources = [
  'gy-text-attribute.c',
  'gy-format-scheme.c',
  'gy-utility-func.c'
]

libgydict_public_headers += files(helpers_headers)
libgydict_public_sources += files(helpers_sources)

install_headers(helpers_headers, install_dir: join_paths(libgydict_header_dir, 'helpers'))
