## Docs

### XO Colors
- Create and manage Sugar XO buddy colors
- Parse color specifications from strings (hex and named colors)
- Convert colors to string representations
- Support for both hex (#FF0000,#00FF00) and RGB (rgb(255,0,0),rgb(0,255,0)) formats

### Utility Functions
- Grid cell size calculation for Sugar's grid-based layout
- Touch-friendly scaling for UI elements
- Touch device detection

## API Reference

### XO Colors API
```c
SugarXOColors* sugar_xo_colors_new(void);
SugarXOColors* sugar_xo_colors_new_from_string(const gchar *color_spec);
void sugar_xo_colors_free(SugarXOColors *colors);
gchar* sugar_xo_colors_to_string(SugarXOColors *colors);
```

### Utility Functions API
```c
gdouble sugar_get_grid_cell_size(void);
gdouble sugar_scale_for_touch(gdouble size);
gboolean sugar_is_touch_device(void);
```
