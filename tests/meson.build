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

# Utility functions specific test
test_utilities = executable('test_utilities',
  'test_utilities.c',
  dependencies: sugar_lib_dep,
  install: false,
)

# Sugar Grid specific test
test_sugar_grid = executable('test_sugar_grid',
  'test_sugar_grid.c',
  dependencies: sugar_lib_dep,
  install: false,
)

# Sugar File Attributes specific test
test_sugar_file_attributes = executable('test_sugar_file_attributes',
  'test_sugar_file_attributes.c',
  dependencies: sugar_lib_dep,
  install: false,
)

# Sugar Event Controller specific test
test_sugar_event_controller = executable('test_sugar_event_controller',
  'test_sugar_event_controller.c',
  dependencies: sugar_lib_dep,
  install: false,
)

# Sugar Long Press Controller specific test
test_sugar_long_press_controller = executable('test_sugar_long_press_controller',
  'test_sugar_long_press_controller.c',
  dependencies: sugar_lib_dep,
  install: false,
)

# Register tests with meson
test('main_functionality', test_main)
test('utilities', test_utilities)
test('sugar_grid', test_sugar_grid)
test('sugar_file_attributes', test_sugar_file_attributes)
test('sugar_event_controller', test_sugar_event_controller)
test('sugar_long_press_controller', test_sugar_long_press_controller)
