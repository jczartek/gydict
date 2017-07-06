sources_h = [
  'egg-binding-group.h',
  'egg-menu-manager.h'
]

sources_c = [
  'egg-binding-group.c',
  'egg-menu-manager.c'
]

libegg_inc = include_directories('.')

libegg_lib = static_library('egg',
                            sources: sources_h + sources_c,
                            dependencies: [gio_dep, gtk_dep],
                            include_directories: libegg_inc)

libegg_dep = declare_dependency (link_with: libegg_lib,
                                 include_directories: libegg_inc)
