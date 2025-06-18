controllers_sources = [
  'sugar-event-controller.c',
  'sugar-long-press-controller.c',
]

controllers_headers = [
  'sugar-event-controller.h',
  'sugar-long-press-controller.h',
  'sugar-event-controllers.h',
]

install_headers(controllers_headers,
  subdir: 'sugar-ext-@0@/sugar/controllers'.format(api_version))

controllers_dep = declare_dependency(
  sources: controllers_sources,
  include_directories: include_directories('.'),
)
