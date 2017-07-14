apiversion = '1.0'
libgydict_header_dir = join_paths(gydict_includedir, 'libgydict-' + apiversion)

libgydict_resources = gnome.compile_resources(
  'gy-resources',
  './resources/gy.gresource.xml',
  c_name: 'gy'
)

install_headers('gydict.h', install_dir: libgydict_header_dir)

libgydict_public_headers =  []
libgydict_public_sources =  []
libgydict_private_sources = []

subdir('app')
subdir('css')
subdir('deflist')
subdir('dictionaries')
subdir('entryview')
subdir('helpers')
subdir('preferences')
subdir('printing')
subdir('search')
subdir('shortcuts')
subdir('window')

libgydct_sources = [
  libgydict_public_sources,
  libgydict_private_sources,
  libgydict_resources
]

libgydict_deps = [
  gtk_dep,
  gio_dep,
  cc.find_library('m',    required: true),
  cc.find_library('zlib', required: true)
]
