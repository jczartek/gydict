version_data = configuration_data()
version_data.set('MAJOR_VERSION', 0)
version_data.set('MINOR_VERSION', 1)
version_data.set('MICRO_VERSION', 0)
version_data.set('VERSION', '0.1.0')

version_h = configure_file(input: 'pnl-version.h.in',
                          output: 'pnl-version.h',
                   configuration: version_data)

resources = gnome.compile_resources('pnl-resources',
                                    'pnl.gresource.xml',
                                    c_name: 'pnl')

pnl_headers = [
  'pnl-animation.h',
  'pnl-dock-bin-edge.h',
  'pnl-dock-bin.h',
  'pnl-dock.h',
  'pnl-dock-item.h',
  'pnl-dock-manager.h',
  'pnl-dock-overlay.h',
  'pnl-dock-paned.h',
  'pnl-dock-revealer.h',
  'pnl-dock-stack.h',
  'pnl-dock-tab-strip.h',
  'pnl-dock-transient-grab.h',
  'pnl-dock-types.h',
  'pnl-dock-widget.h',
  'pnl-dock-window.h',
  'pnl-frame-source.h',
  'pnl-multi-paned.h',
  'pnl-resources.h',
  'pnl-tab.h',
  'pnl-tab-strip.h',
  'pnl.h'
]

pnl_generated_files = resources + [
  version_h
]

pnl_headers_private = [
  'pnl-dock-bin-edge-private.h',
  'pnl-dock-overlay-edge-private.h',
  'pnl-dock-paned-private.h',
  'pnl-util-private.h'
]

pnl_source = pnl_headers + pnl_headers_private + pnl_generated_files +[
  'pnl-animation.c',
  'pnl-dock-bin.c',
  'pnl-dock-bin-edge.c',
  'pnl-dock.c',
  'pnl-dock-item.c',
  'pnl-dock-manager.c',
  'pnl-dock-overlay.c',
  'pnl-dock-overlay-edge.c',
  'pnl-dock-paned.c',
  'pnl-dock-revealer.c',
  'pnl-dock-stack.c',
  'pnl-dock-tab-strip.c',
  'pnl-dock-transient-grab.c',
  'pnl-dock-widget.c',
  'pnl-dock-window.c',
  'pnl-frame-source.c',
  'pnl-multi-paned.c',
  'pnl-resources.c',
  'pnl-tab.c',
  'pnl-tab-strip.c',
  'pnl-util.c'
]

pnl_args = [
  '-DPNL_COMPILATION'
]

pnl_inc = include_directories('.')

pnl_lib = static_library('pnl',
                        sources: pnl_source,
                   dependencies: [gio_dep, gtk_dep],
                         c_args: pnl_args,
            include_directories: pnl_inc)

pnl_dep = declare_dependency(link_with: pnl_lib,
                   include_directories: pnl_inc)
