# Sugar-Ext

The sugar4 library contains a set of widgets to build HIG compliant applications and interfaces to interact with system services like presence and the datastore.

## Building

This project uses Meson build system:

```bash
meson setup builddir
meson compile -C builddir
```

## Testing

Run the test suite:

```bash
meson test -C builddir
```

Tests include:

- `main_functionality`: Comprehensive test of all features
- `utilities`: Utility functions testing
- `sugar_grid`: Sugar Grid spatial layout system testing
- `sugar_event_controller`: Tests for event handling and custom event controller logic
- `sugar_file_attributes`: Tests for file attribute management and metadata handling
- `sugar_long_press_controller`: Handling the delayed controlling and senses.

## Installation

```bash
meson install -C builddir
```

## Examples > [!WARNING]

```bash
meson compile -C builddir 
# then follow by the example you want to run
./builddir/examples/sugar_grid_example
./builddir/examples/sugar_event_controller_example
./builddir/examples/sugar_file_attributes_example
```

## Dependencies

- GTK4 >= 4.0
- GLib >= 2.70
- GObject
- GIO
