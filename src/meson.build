api_version = '0.1'

sugar_ext_sources = [
  'sugar-ext.c',
]

sugar_ext_headers = [
  'sugar-ext.h',
]

version_split = meson.project_version().split('.')
version_conf = configuration_data()
version_conf.set('VERSION', meson.project_version())
version_conf.set('MAJOR_VERSION', version_split[0])
version_conf.set('MINOR_VERSION', version_split[1])
version_conf.set('MICRO_VERSION', version_split[2])

configure_file(
          input: 'sugar-ext-version.h.in',
         output: 'sugar-ext-version.h',
  configuration: version_conf,
        install: true,
    install_dir: get_option('includedir') / 'sugar-ext'
)

sugar_ext_deps = [
  dependency('gtk4', version: '>= 4.0'),
  dependency('glib-2.0', version: '>= 2.70'),
  dependency('gobject-2.0'),
  dependency('gio-2.0'),
]

sugar_ext_lib = shared_library('sugar-ext-' + api_version,
  sugar_ext_sources,
  dependencies: sugar_ext_deps,
       install: true,
)

install_headers(sugar_ext_headers, subdir: 'sugar-ext')

pkg = import('pkgconfig')
pkg.generate(
  description: 'Sugar Toolkit GTK4 C Library - Core Sugar desktop environment integration',
    libraries: sugar_ext_lib,
         name: 'sugar-ext',
     filebase: 'sugar-ext-' + api_version,
      version: meson.project_version(),
      subdirs: 'sugar-ext',
     requires: ['gtk4', 'glib-2.0', 'gobject-2.0', 'gio-2.0'],
  install_dir: get_option('libdir') / 'pkgconfig'
)

gnome = import('gnome')
sugar_ext_gir = gnome.generate_gir(sugar_ext_lib,
              sources: [sugar_ext_sources, sugar_ext_headers],
               header: 'sugar-ext.h',
      export_packages: 'sugar-ext-' + api_version,
            nsversion: api_version,
            namespace: 'SugarExt',
        symbol_prefix: 'sugarext',
    identifier_prefix: 'SugarExt',
             includes: ['Gtk-4.0', 'Gio-2.0'],
              install: true,
)
