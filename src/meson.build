exe_link_args = [
  '-pie',
]

exe_c_args = [
  '-fPIE',
]

executable('gydict', 'gy-main.c',
              gui_app: true,
              install: true,
               c_args: exe_c_args,
            link_args: exe_link_args,
        install_rpath: gydict_pkglibdir_abs,
         dependencies: libgydict_dep,
  include_directories: root_dir
)
