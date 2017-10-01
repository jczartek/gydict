dictionaries_headers = [
  'gy-depl.h',
  'gy-dict.h',
  'gy-dict-manager.h',
  'gy-english-pwn.h',
  'gy-entry-collector.h',
  'gy-german-pwn.h',
  'gy-parsable.h',
  'gy-parser-pwn.h',
  'gy-pwn-dict.h',
  'gy-pwntabs.h'
]

dictionaries_sources = [
  'gy-depl.c',
  'gy-dict.c',
  'gy-dict-manager.c',
  'gy-english-pwn.c',
  'gy-entry-collector.c',
  'gy-german-pwn.c',
  'gy-parsable.c',
  'gy-parser-pwn.c',
  'gy-pwn-dict.c',
  'gy-pwntabs.c'
]

libgydict_public_headers += files(dictionaries_headers)
libgydict_public_sources += files(dictionaries_sources)

install_headers(dictionaries_headers, install_dir: join_paths(libgydict_header_dir, 'dictionaries'))
