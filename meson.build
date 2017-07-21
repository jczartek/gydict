project('gydict', 'c',
        version: '0.4.0',
        default_options: ['c_std=gnu11', 'warning_level=2'],
        license: 'GPLv2+',
        messon_version: '>= 0.39.1')

gydict_prefix     = get_option('prefix')
gydict_bindir     = join_paths(gydict_prefix, get_option('bindir'))
gydict_libdir     = join_paths(gydict_prefix, get_option('libdir'), 'gydict')
gydict_datadir    = join_paths(gydict_prefix, get_option('datadir'))
gydict_includedir = join_paths(gydict_prefix, get_option('includedir'), 'gydict')
gydict_schemadir  = join_paths(gydict_datadir, 'glib-2.0', 'schemas')

gydict_pkglibdir_abs = join_paths(get_option('prefix'), get_option('libdir'), 'gydict')


conf = configuration_data()
conf.set_quoted('PACKAGE_NAME', meson.project_name())
conf.set_quoted('PACKAGE_VERSION', meson.project_version())
conf.set_quoted('PACKAGE_STRING', '@0@-@1@'.format(meson.project_name(), meson.project_version()))
conf.set_quoted('PACKAGE_DATADIR', gydict_datadir)
conf.set_quoted('PACKAGE_LIBDIR', gydict_libdir)
conf.set_quoted('PACKAGE_LOCAL_DIR', join_paths(gydict_datadir, 'locale'))

conf.set('VERSION', 'PACKAGE_VERSION')
conf.set('GETTEXT_PACKAGE', 'PACKAGE_NAME')
conf.set('LOCALEDIR', 'PACKAGE_LOCALE_DIR')

conf.set10('ENABLE_NLS', true)


cc          = meson.get_compiler('c')
host_system = host_machine.system()

add_global_arguments([
  '-DHAVE_CONFIG_H',
  '-I' + meson.build_root(), #config.h
], language: 'c')

test_cflags = []
if cc.get_id() == 'gcc' or cc.get_id() == 'clang'
  test_cflags += [
    '-Wall',
    '-Wcast-align',
    ['-Werror=format-security', '-Werror=format=2'],
    '-Wextra',
    '-Wformat-nonliteral',
    '-Wformat-security',
    '-Wmissing-include-dirs',
    '-Wnested-externs',
    '-Wno-missing-field-initializers',
    '-Wno-sign-compare',
    '-Wno-strict-aliasing',
    '-Wno-unused-parameter',
    '-Wpointer-arith',
    '-Wredundant-decls',
    '-Wshadow',
    '-Wswitch-default',
    '-Wswitch-enum',
    '-Wundef'
    ]
endif

common_clfags = []
foreach cflag: test_cflags
  if cc.has_multi_arguments(cflag)
    common_clfags += [ cflag ]
  endif
endforeach

add_project_arguments(
  common_clfags,
  language: 'c'
)

root_dir = include_directories('.')
po_dir   = join_paths(meson.current_source_dir(), 'po')

configure_file(output: 'config.h', configuration: conf)

gio_dep = dependency('gio-2.0', version: '>= 2.50.0')
gtk_dep = dependency('gtk+-3.0', version: '>= 3.22.0')

gnome = import('gnome')
i18n  = import('i18n')

subdir('contrib/egg')
subdir('contrib/pnl')
subdir('data')
subdir('icons')
subdir('libgydict')
subdir('po')
subdir('src')
subdir('tests')

meson.add_install_script ('build-aux/post-install.sh')

status = [
  '', '',
  'gydict @0@'.format(meson.project_version()),
  '', '',
  'Version .............. : @0@'.format(meson.project_version()),
  '',
  'Prefix ............... : @0@'.format(gydict_prefix),
  'Includedir ........... : @0@'.format(gydict_includedir),
  'Libdir ............... : @0@'.format(gydict_libdir),
  '', ''
]

message('\n'.join(status))
