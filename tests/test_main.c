#include <gtk/gtk.h>
#include <sugar-ext.h>
#include <assert.h>
#include <stdio.h>
#include <string.h>

static void test_xo_colors_new(void) {
    printf("Testing sugar_xo_colors_new...\n");
    
    SugarXOColors *colors = sugar_xo_colors_new();
    assert(colors != NULL);
    
    // chcek that colors are initialized to some values
    assert(colors->fill_color.red >= 0.0 && colors->fill_color.red <= 1.0);
    assert(colors->fill_color.green >= 0.0 && colors->fill_color.green <= 1.0);
    assert(colors->fill_color.blue >= 0.0 && colors->fill_color.blue <= 1.0);
    assert(colors->fill_color.alpha >= 0.0 && colors->fill_color.alpha <= 1.0);
    
    assert(colors->stroke_color.red >= 0.0 && colors->stroke_color.red <= 1.0);
    assert(colors->stroke_color.green >= 0.0 && colors->stroke_color.green <= 1.0);
    assert(colors->stroke_color.blue >= 0.0 && colors->stroke_color.blue <= 1.0);
    assert(colors->stroke_color.alpha >= 0.0 && colors->stroke_color.alpha <= 1.0);
    
    sugar_xo_colors_free(colors);
    printf("PASSED: sugar_xo_colors_new\n");
}

static void test_xo_colors_from_string(void) {
    printf("Testing sugar_xo_colors_new_from_string...\n");
    
    SugarXOColors *colors = sugar_xo_colors_new_from_string("#FF0000,#0000FF");
    assert(colors != NULL);
    
    // Red fill color (approximately)
    assert(colors->fill_color.red > 0.9);
    assert(colors->fill_color.green < 0.1);
    assert(colors->fill_color.blue < 0.1);
    
    // Blue stroke color (approximately)
    assert(colors->stroke_color.red < 0.1);
    assert(colors->stroke_color.green < 0.1);
    assert(colors->stroke_color.blue > 0.9);
    
    sugar_xo_colors_free(colors);
    
    // Test with invalid specification
    colors = sugar_xo_colors_new_from_string("invalid");
    // Should either return NULL or fallback colors - both are acceptable
    if (colors != NULL) {
        sugar_xo_colors_free(colors);
    }
    
    printf("PASSED: sugar_xo_colors_new_from_string\n");
}

static void test_xo_colors_to_string(void) {
    printf("Testing sugar_xo_colors_to_string...\n");
    
    SugarXOColors *colors = sugar_xo_colors_new_from_string("#FF0000,#0000FF");
    assert(colors != NULL);
    
    gchar *color_string = sugar_xo_colors_to_string(colors);
    assert(color_string != NULL);
    assert(strlen(color_string) > 0);
    
    printf("Color string: %s\n", color_string);
    
    g_free(color_string);
    sugar_xo_colors_free(colors);
    
    printf("PASSED: sugar_xo_colors_to_string\n");
}

static void test_utility_functions(void) {
    printf("Testing utility functions...\n");
    
    // Test grid cell size
    gdouble cell_size = sugar_get_grid_cell_size();
    assert(cell_size > 0.0);
    printf("Grid cell size: %f\n", cell_size);
    
    // Test scaling for touch
    gdouble scaled = sugar_scale_for_touch(10.0);
    assert(scaled > 0.0);
    printf("Scaled size (10.0 -> %f)\n", scaled);
    
    // Test touch device detection
    gboolean has_touch = sugar_is_touch_device();
    printf("Has touch device: %s\n", has_touch ? "yes" : "no");
    
    printf("PASSED: utility functions\n");
}

int main(int argc, char *argv[]) {
    // initializing gtk here because it's required for GdkRGBA 
    gtk_init();
    
    printf("=== Sugar Toolkit GTK4 C Library Tests ===\n\n");
    
    test_xo_colors_new();
    test_xo_colors_from_string();
    test_xo_colors_to_string();
    test_utility_functions();
    
    printf("\n=== All tests passed! ===\n");
    
    return 0;
}
