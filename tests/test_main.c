#include <gtk/gtk.h>
#include <sugar-ext.h>
#include <assert.h>
#include <stdio.h>
#include <string.h>

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
    // initializing gtk here because it's required for touch detection 
    gtk_init();
    
    printf("=== Sugar Toolkit GTK4 C Library Tests ===\n\n");
    
    test_utility_functions();
    
    printf("\n=== All tests passed! ===\n");
    
    return 0;
}
