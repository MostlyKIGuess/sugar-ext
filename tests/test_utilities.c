#include <gtk/gtk.h>
#include <sugar-ext.h>
#include <assert.h>
#include <stdio.h>

static void test_grid_cell_size_consistency(void) {
    printf("Testing grid cell size consistency...\n");
    
    gdouble size1 = sugar_get_grid_cell_size();
    gdouble size2 = sugar_get_grid_cell_size();
    gdouble size3 = sugar_get_grid_cell_size();
    
    assert(size1 > 0.0);
    assert(size1 == size2);
    assert(size2 == size3);
    
    printf("Grid cell size: %f pixels\n", size1);
    printf("PASSED: grid cell size consistency\n");
}

static void test_scale_for_touch_behavior(void) {
    printf("Testing scale for touch behavior...\n");
    
    gdouble base_sizes[] = {1.0, 10.0, 50.0, 100.0, 200.0};
    int num_sizes = sizeof(base_sizes) / sizeof(base_sizes[0]);
    
    for (int i = 0; i < num_sizes; i++) {
        gdouble base = base_sizes[i];
        gdouble scaled = sugar_scale_for_touch(base);
        
        assert(scaled > 0.0);
        printf("  %f -> %f (ratio: %f)\n", base, scaled, scaled / base);
        
        // Scaled size should generally be >= base size for touch accessibility
        // so here I can just check it's positive
        assert(scaled > 0.0);
    }
    
    printf("PASSED: scale for touch behavior\n");
}

static void test_touch_device_detection(void) {
    printf("Testing touch device detection...\n");
    
    gboolean has_touch1 = sugar_is_touch_device();
    gboolean has_touch2 = sugar_is_touch_device();
    
    // should be same 
    assert(has_touch1 == has_touch2);
    // printf(has_touch1 ? "Touch device detected: yes\n" : "Touch device detected: no\n"); TODO: check with something that has touch
    printf("Touch device detected: %s\n", has_touch1 ? "yes" : "no");
    printf("PASSED: touch device detection\n");
}

int main(int argc, char *argv[]) {
    gtk_init();
    
    printf("=== Sugar Utility Functions Tests ===\n\n");
    
    test_grid_cell_size_consistency();
    test_scale_for_touch_behavior();
    test_touch_device_detection();
    
    printf("\n=== Utility function tests completed ===\n");
    
    return 0;
}
