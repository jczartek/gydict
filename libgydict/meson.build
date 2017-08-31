apiversion = '1.0'
libgydict_header_dir = join_paths(gydict_includedir, 'libgydict-' + apiversion)

install_headers('gydict.h', install_dir: libgydict_header_dir)

libgydict_public_headers =  []
libgydict_public_sources =  []
libgydict_private_sources = ['gy-dict-debug.h']

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
  libpattern_dep,
  pnl_dep,
  cc.find_library('m',    required: true),
  cc.find_library('z', required: true)
]

libgydict = shared_library('gydict-' + apiversion, libgydct_sources,
         dependencies: libgydict_deps,
              install: true,
               c_args: debug_cflags,
        install_rpath: gydict_pkglibdir_abs,
          install_dir: gydict_libdir)

libgydict_dep = declare_dependency(
            dependencies: libgydict_deps,
               link_with: libgydict,
     include_directories: include_directories('.'),
)

pkgg = import('pkgconfig')

pkgg.generate(
    libraries: [libgydict],
      subdirs: ['gydict-@0@/libgydict'.format(meson.project_version()) ],
      version: meson.project_version(),
         name: 'Libgydict',
     filebase: 'libgydict-1.0',
  description: 'Libgydict contains the components used to build the application Gydict.',
     requires: ['gtk+-3.0'],
  install_dir: join_paths(gydict_libdir, 'pkgconfig'),
)
