apiversion = '2.0'
libgydict_header_dir = join_paths(gydict_includedir, 'libgydict-' + apiversion)

install_headers('gydict.h', install_dir: libgydict_header_dir)

libgydict_public_headers =  []
libgydict_public_sources =  []
libgydict_private_sources = ['gy-dict-debug.h']
libgydict_private_headers = []

subdir('app')
subdir('helpers')
#subdir('preferences')
#subdir('resources')
subdir('services')
#subdir('gui')

libgydct_sources = [
 libgydict_public_sources,
 libgydict_private_sources,
 #libgydict_resources
]

libgydict_deps = [
  gtk4_dep,
  adwaita_dep,
  gio_dep,
  peas_dep,
  cc.find_library('m', required: true),
  cc.find_library('z', required: true)
]

# Enumerations for GType
libgydict_enums = gnome.mkenums(
  'gy-enum-types',
  sources: libgydict_public_headers,
  c_template: 'gy-enum-types.c.template',
  h_template: 'gy-enum-types.h.template',
  install_dir: libgydict_header_dir,
  install_header: true,)

libgydict = shared_library('gydict-' + apiversion, libgydct_sources + libgydict_enums,
         dependencies: libgydict_deps,
              install: true,
               c_args: debug_cflags + ['-DGYDICT_COMPILATION'],
        install_rpath: gydict_pkglibdir_abs,
          install_dir: gydict_libdir)

libgydict_dep = declare_dependency(
                 sources: libgydict_private_headers,
            dependencies: libgydict_deps,
               link_with: libgydict,
     include_directories: include_directories('.'),
)

pkgg = import('pkgconfig')

pkgg.generate(
    libraries: [libgydict],
      subdirs: [join_paths('gydict', 'libgydict-' + apiversion) ],
      version: meson.project_version(),
         name: 'libgydict',
     filebase: 'libgydict-' + apiversion,
  description: 'Libgydict contains the components used to build the application Gydict.',
     requires: ['gtk-4.0', 'libadwaita-1', 'gio-2.0', 'libpeas-1.0'],
  install_dir: join_paths(gydict_libdir, 'pkgconfig'),
    variables: ['vapidir=@0@'.format(gydict_vapidir)],
)

# libgydict_gir = gnome.generate_gir(libgydict,
#               sources: libgydict_public_headers + libgydict_public_sources + [ libgydict_enums[1] ] ,
#             nsversion: apiversion,
#             namespace: 'Gydict',
#         symbol_prefix: 'gy',
#     identifier_prefix: 'Gy',
#              includes: [ 'Gio-2.0', 'Gtk-3.0', 'Dazzle-1.0' ],
#               install: true,
#   install_dir_typelib: gydict_typelibdir,
#       install_dir_gir: gydict_girdir,
#            extra_args: ['--c-include=gydict.h', '--quiet', '-DGYDICT_COMPILATION'],)

# libgydict_vapi = gnome.generate_vapi(
#     'libgydict-' + apiversion,
#       install: true,
#       install_dir: gydict_vapidir,
#       sources: libgydict_gir[0],
#       metadata_dirs: [ meson.current_source_dir() ],
#       packages: [ 'gio-2.0', 'gtk+-3.0', 'libdazzle-1.0'])
    
