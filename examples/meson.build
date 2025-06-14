
sugar_ext_deps = [
  dependency('gtk4', version: '>= 4.0'),
  dependency('glib-2.0', version: '>= 2.70'),
  dependency('gobject-2.0'),
  dependency('gio-2.0'),
]

sugar_lib_dep = declare_dependency(
  link_with: sugar_ext_lib,
  include_directories: include_directories('../src'),
  dependencies: sugar_ext_deps
)

sugar_grid_example = executable('sugar_grid_example',
  'sugar_grid_example.c',
  dependencies: sugar_lib_dep,
  install: false,
)
