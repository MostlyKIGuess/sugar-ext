
sugar_ext_deps = [
  dependency('gtk4', version: '>= 4.0'),
  dependency('glib-2.0', version: '>= 2.70'),
  dependency('gobject-2.0'),
  dependency('gio-2.0'),
]

# Math library dependency
math_dep = meson.get_compiler('c').find_library('m', required: false)

sugar_lib_dep = declare_dependency(
  link_with: sugar_ext_lib,
  include_directories: include_directories('../src'),
  dependencies: sugar_ext_deps + [math_dep]
)

sugar_grid_example = executable('sugar_grid_example',
  'sugar_grid_example.c',
  dependencies: sugar_lib_dep,
  install: false,
)

sugar_file_attributes_example = executable('sugar_file_attributes_example',
  'sugar_file_attributes_example.c',
  dependencies: sugar_lib_dep,
  install: false,
)

sugar_event_controller_example = executable('sugar_event_controller_example',
  'sugar_event_controller_example.c',
  dependencies: sugar_lib_dep,
  install: false,
)

sugar_long_press_controller_example = executable('sugar_long_press_controller_example',
  'sugar_long_press_controller_example.c',
  dependencies: sugar_lib_dep,
  install: false,
)
