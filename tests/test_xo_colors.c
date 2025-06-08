#include <gtk/gtk.h>
#include <sugar-ext.h>
#include <assert.h>
#include <stdio.h>
#include <string.h>
#include <math.h>

static void test_colors_roundtrip(void) {
    printf("Testing XO Colors roundtrip conversion...\n");

    SugarXOColors *original = sugar_xo_colors_new_from_string("#FF0000,#00FF00");
    assert(original != NULL);
    
    gchar *color_string = sugar_xo_colors_to_string(original);
    assert(color_string != NULL);
    printf("Generated color string: '%s'\n", color_string);
    
    // string -> color
    SugarXOColors *roundtrip = sugar_xo_colors_new_from_string(color_string);
    if (roundtrip == NULL) {
        printf("Failed to parse generated color string: '%s'\n", color_string);
        
        gchar **parts = g_strsplit(color_string, ",", 2);
        if (g_strv_length(parts) == 2) {
            printf("Part 1: '%s'\n", parts[0]);
            printf("Part 2: '%s'\n", parts[1]);
            
            GdkRGBA test_color;
            gboolean parse1 = gdk_rgba_parse(&test_color, g_strstrip(parts[0]));
            gboolean parse2 = gdk_rgba_parse(&test_color, g_strstrip(parts[1]));
            printf("Parse results: %s, %s\n", parse1 ? "OK" : "FAIL", parse2 ? "OK" : "FAIL");
        }
        g_strfreev(parts);
        
        g_free(color_string);
        sugar_xo_colors_free(original);
        assert(roundtrip != NULL); // This will fail and show us what happened
        return;
    }
    
    // colors comparison (with some tolerance for rounding)
    assert(fabs(original->fill_color.red - roundtrip->fill_color.red) < 0.01);
    assert(fabs(original->fill_color.green - roundtrip->fill_color.green) < 0.01);
    assert(fabs(original->fill_color.blue - roundtrip->fill_color.blue) < 0.01);
    
    assert(fabs(original->stroke_color.red - roundtrip->stroke_color.red) < 0.01);
    assert(fabs(original->stroke_color.green - roundtrip->stroke_color.green) < 0.01);
    assert(fabs(original->stroke_color.blue - roundtrip->stroke_color.blue) < 0.01);
    
    printf("Original: %s\n", color_string);
    
    g_free(color_string);
    sugar_xo_colors_free(original);
    sugar_xo_colors_free(roundtrip);
    
    printf("PASSED: XO Colors roundtrip conversion\n");
}

static void test_various_color_formats(void) {
    printf("Testing various color formats...\n");
    
    const char *test_formats[] = {
        "#FF0000,#0000FF",      // Standard hex
        "#ff0000,#0000ff",      // Lowercase hex
        "#F00,#00F",            // Short hex
        NULL
    };
    
    for (int i = 0; test_formats[i] != NULL; i++) {
        printf("Testing format: %s\n", test_formats[i]);
        SugarXOColors *colors = sugar_xo_colors_new_from_string(test_formats[i]);
        
        if (colors != NULL) {
            gchar *result = sugar_xo_colors_to_string(colors);
            printf("  Result: %s\n", result ? result : "NULL");
            g_free(result);
            sugar_xo_colors_free(colors);
        } else {
            printf("  Result: NULL (format not supported)\n");
        }
    }
    
    printf("PASSED: various color formats\n");
}

int main(int argc, char *argv[]) {
    gtk_init();
    
    printf("=== Sugar XO Colors Advanced Tests ===\n\n");
    
    test_colors_roundtrip();
    test_various_color_formats();
    
    printf("\n=== XO Colors tests completed ===\n");
    
    return 0;
}
