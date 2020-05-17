services_headers = [
  'gy-service.h',
  'gy-dict-formatter.h',
  'gy-dict-service.h',
  'gy-service-provider.h'
]

services_sources = [
  'gy-service.c',
  'gy-dict-formatter.c',
  'gy-dict-service.c',
  'gy-service-provider.c'
]

libgydict_public_headers += files(services_headers)
libgydict_public_sources += files(services_sources)

install_headers(services_headers, install_dir: join_paths(libgydict_header_dir, 'services'))
