# Get dependencies for tests
sugar_ext_deps = [
  dependency('gtk4', version: '>= 4.0'),
  dependency('glib-2.0', version: '>= 2.70'),
  dependency('gobject-2.0'),
  dependency('gio-2.0'),
]

# Use the library from the src directory
sugar_lib_dep = declare_dependency(
  link_with: sugar_ext_lib,
  include_directories: include_directories('../src'),
  dependencies: sugar_ext_deps
)

# Main comprehensive test
test_main = executable('test_main',
  'test_main.c',
  dependencies: sugar_lib_dep,
  install: false,
)

# XO Colors specific test
test_xo_colors = executable('test_xo_colors',
  'test_xo_colors.c',
  dependencies: sugar_lib_dep,
  install: false,
)

# Utility functions specific test
test_utilities = executable('test_utilities',
  'test_utilities.c',
  dependencies: sugar_lib_dep,
  install: false,
)

# Register tests with meson
test('main_functionality', test_main)
test('xo_colors', test_xo_colors)
test('utilities', test_utilities)
