exe_link_args = []

exe_c_args = []

executable('gydict', 'gy-main.c',
        win_subsystem: 'windows',
              install: true,
            link_with: [libgydict],
               c_args: exe_c_args,
            link_args: exe_link_args,
        install_rpath: gydict_pkglibdir_abs,
         dependencies: libgydict_dep,
  include_directories: root_dir
)
