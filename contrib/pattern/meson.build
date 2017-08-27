sources_h = [
  'g-observable.h',
  'g-observer.h',
  'g-pattern.h'
]

sources_private = [
  'g-pattern-types.h'
]

sources_c = [
  'g-observable.c',
  'g-observer.c'
]

libpattern_inc = include_directories('.')

libpattern_deps = [
  gio_dep,
  gtk_dep
]

libpattern_lib = shared_library('pattern-private',
              sources: sources_private + sources_h + sources_c,
         dependencies: libpattern_deps,
  include_directories: libpattern_inc,
              install: true,
          install_dir: gydict_libdir)

libpattern_dep = declare_dependency (
            link_with: libpattern_lib,
         dependencies: libpattern_deps,
  include_directories: libpattern_inc)
