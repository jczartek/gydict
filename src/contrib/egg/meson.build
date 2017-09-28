sources_h = [
  'egg-binding-group.h',
  'egg-menu-manager.h'
]

install_headers(sources_h, subdir: 'gydict/egg')

sources_c = [
  'egg-binding-group.c',
  'egg-menu-manager.c'
]

libegg_inc = include_directories('.')

lib_deps = [
  gio_dep,
  gtk_dep
]

libegg_lib = shared_library('egg-private',
              sources: sources_h + sources_c,
         dependencies: lib_deps,
  include_directories: libegg_inc,
              install: true,
          install_dir: gydict_libdir)

libegg_dep = declare_dependency (
            link_with: libegg_lib,
         dependencies: lib_deps,
  include_directories: libegg_inc)
