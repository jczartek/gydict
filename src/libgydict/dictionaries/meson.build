dictionaries_headers = [
  'gy-dict.h',
  'gy-dict-manager.h',
]

dictionaries_sources = [
  'gy-dict.c',
  'gy-dict-manager.c',
]

libgydict_public_headers += files(dictionaries_headers)
libgydict_public_sources += files(dictionaries_sources)

install_headers(dictionaries_headers, install_dir: join_paths(libgydict_header_dir, 'dictionaries'))
