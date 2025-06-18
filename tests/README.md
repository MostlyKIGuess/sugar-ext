# Sugar Toolkit GTK4 Tests

This directory contains tests for the Sugar Toolkit GTK4 C library.

## How to Run Tests

Tests are run using the Meson build system. From the root of the project, run:

```bash
meson test -C builddir
```

## Current Status

As of the last update, the following tests are included:

- `test_main`: Tests the main functionality of the library.
- `test_sugar_grid`: Tests the `SugarGrid` widget.
- `test_sugar_file_attributes`: Tests the `SugarFileAttributes` utility.
- `test_utilities`: Tests various utility functions.
- `test_sugar_event_controller`: Tests the public API of the abstract `SugarEventController`.
- `test_sugar_long_press_controller`: Tests the public API of the `SugarLongPressController`.

All tests pass except for `test_sugar_event_controller` and `test_sugar_long_press_controller`.

## Known Issues: SIGTRAP in Controller Tests

The tests for `SugarEventController` and `SugarLongPressController` are currently failing with a `SIGTRAP` signal. This indicates a crash within the underlying GTK or GObject libraries.

### What has been done:

- The tests have been carefully written to only test the public API of the controllers (creation, attachment, state, signal connection) without relying on event simulation, which was the initial suspected cause of the crash.
- The tests include checks to ensure a display server is available before creating any GTK widgets.

### Hypothesis:

The crash is likely due to a subtle issue in the GObject subclassing of `GtkEventController` or the way the test environment is set up. It is difficult to debug without deeper expertise in GTK internals.

**We are actively seeking help from a GTK/GObject expert to resolve this issue.**

Until then, the controller tests are expected to fail. The code for the controllers themselves has been manually verified and appears correct, but the unit tests cannot be made to pass at this time.
