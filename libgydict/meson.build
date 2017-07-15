apiversion = '1.0'
libgydict_header_dir = join_paths(gydict_includedir, 'libgydict-' + apiversion)

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
subdir('resources')
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
  libegg_dep,
  pnl_dep,
  cc.find_library('m',    required: true),
  cc.find_library('z', required: true)
]

libgydict = shared_library('gydict-' + apiversion, libgydct_sources,
         dependencies: libgydict_deps,
              install: true,
          install_dir: gydict_libdir,
  include_directories: [root_dir])

libgydict_dep = declare_dependency(
            dependencies: libgydict_deps,
               link_with: libgydict,
     include_directories: include_directories('.'),
)
