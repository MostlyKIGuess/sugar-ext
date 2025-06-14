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
- `xo_colors`: Advanced XO Colors functionality testing
- `utilities`: Utility functions testing
- `sugar_grid`: Sugar Grid spatial layout system testing

## Installation

```bash
meson install -C builddir
```

## Examples > [!WARNING]

```bash
meson compile -C builddir 
# then follow by the example you want to run
./builddir/examples/example_name
```

## Dependencies

- GTK4 >= 4.0
- GLib >= 2.70
- GObject
- GIO
