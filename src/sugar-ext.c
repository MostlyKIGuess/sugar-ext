/* sugar-ext.c
 *
 * Copyright 2025 MostlyK
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <https://www.gnu.org/licenses/>.
 *
 * SPDX-License-Identifier: LGPL-2.1
 */

#define SUGAR_EXT_C_COMPILATION
#include "sugar-ext.h"
#include <string.h>
#include <stdlib.h>

#define SUGAR_GRID_CELL_SIZE 75.0
#define MIN_TOUCH_SIZE 44.0

/**
 * sugar_xo_colors_new:
 *
 * Creates a new #SugarXOColors structure with default black/white colors.
 *
 * Returns: (transfer full): A newly allocated #SugarXOColors. Free with sugar_xo_colors_free().
 */
SugarXOColors*
sugar_xo_colors_new(void)
{
    SugarXOColors *colors = g_new0(SugarXOColors, 1);
    
    /* Default to black fill, white stroke */
    gdk_rgba_parse(&colors->fill_color, "#000000");
    gdk_rgba_parse(&colors->stroke_color, "#FFFFFF");
    
    return colors;
}

/**
 * sugar_xo_colors_new_from_string:
 * @color_spec: A color specification string in format "fill,stroke" (e.g., "#FF0000,#00FF00")
 *
 * Creates a new #SugarXOColors structure from a color specification string.
 *
 * Returns: (transfer full): A newly allocated #SugarXOColors or %NULL if parsing failed.
 */
SugarXOColors*
sugar_xo_colors_new_from_string(const gchar* color_spec)
{
    SugarXOColors *colors;
    gchar **parts;
    gchar *comma_pos;
    gchar *first_color, *second_color;
    
    g_return_val_if_fail(color_spec != NULL, NULL);
    
    colors = g_new0(SugarXOColors, 1);
    
    // Handle different formats: hex (#FF0000,#00FF00) vs rgb (rgb(255,0,0),rgb(0,255,0))
    if (g_str_has_prefix(color_spec, "rgb(")) {
        // For RGB format, we need to find the comma between the two rgb() expressions
        // Look for "),rgb(" pattern
        comma_pos = g_strstr_len(color_spec, -1, "),rgb(");
        if (comma_pos == NULL) {
            g_free(colors);
            return NULL;
        }
        
        // split at commas
        first_color = g_strndup(color_spec, comma_pos - color_spec + 1);
        second_color = g_strdup(comma_pos + 2);
        
    } else {
        // For hex format or other simple formats, split on first comma
        parts = g_strsplit(color_spec, ",", 2);
        
        if (g_strv_length(parts) != 2) {
            g_strfreev(parts);
            g_free(colors);
            return NULL;
        }
        
        first_color = g_strdup(g_strstrip(parts[0]));
        second_color = g_strdup(g_strstrip(parts[1]));
        g_strfreev(parts);
    }
    
    if (!gdk_rgba_parse(&colors->fill_color, first_color) ||
        !gdk_rgba_parse(&colors->stroke_color, second_color)) {
        g_free(first_color);
        g_free(second_color);
        g_free(colors);
        return NULL;
    }
    
    g_free(first_color);
    g_free(second_color);
    return colors;
}

/**
 * sugar_xo_colors_free:
 * @colors: A #SugarXOColors
 *
 * Frees a #SugarXOColors structure.
 */
void
sugar_xo_colors_free(SugarXOColors* colors)
{
    g_free(colors);
}

/**
 * sugar_xo_colors_to_string:
 * @colors: A #SugarXOColors
 *
 * Converts a #SugarXOColors to its string representation.
 *
 * Returns: (transfer full): A newly allocated string. Free with g_free().
 */
gchar*
sugar_xo_colors_to_string(SugarXOColors* colors)
{
    gchar *fill_str;
    gchar *stroke_str;
    gchar *result;
    
    g_return_val_if_fail(colors != NULL, NULL);
    
    fill_str = gdk_rgba_to_string(&colors->fill_color);
    stroke_str = gdk_rgba_to_string(&colors->stroke_color);
    result = g_strdup_printf("%s,%s", fill_str, stroke_str);
    
    g_free(fill_str);
    g_free(stroke_str);
    
    return result;
}

/**
 * sugar_get_grid_cell_size:
 *
 * Gets the standard Sugar grid cell size.
 *
 * Returns: The grid cell size in pixels.
 */
gdouble
sugar_get_grid_cell_size(void)
{
    return SUGAR_GRID_CELL_SIZE;
}

/**
 * sugar_scale_for_touch:
 * @base_size: The base size to scale
 *
 * Scales a size to be more touch-friendly.
 *
 * Returns: The scaled size, ensuring minimum 44px for touch targets.
 */
gdouble
sugar_scale_for_touch(gdouble base_size)
{
    return MAX(base_size, MIN_TOUCH_SIZE);
}

/**
 * sugar_is_touch_device:
 *
 * Determines if the current device has touch input capabilities.
 *
 * Returns: %TRUE if touch is available, %FALSE otherwise.
 */
gboolean
sugar_is_touch_device(void)
{
    GdkDisplay *display;
    GList *seats, *l;
    
    display = gdk_display_get_default();
    if (!display)
        return FALSE;
    
    /* Check if any seat has touch capability */
    seats = gdk_display_list_seats(display);
    
    for (l = seats; l != NULL; l = l->next) {
        GdkSeat *seat = GDK_SEAT(l->data);
        GdkSeatCapabilities caps = gdk_seat_get_capabilities(seat);
        
        if (caps & GDK_SEAT_CAPABILITY_TOUCH) {
            g_list_free(seats);
            return TRUE;
        }
    }
    
    g_list_free(seats);
    return FALSE;
}

