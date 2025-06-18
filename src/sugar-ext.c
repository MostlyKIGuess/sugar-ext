/* sugar-ext.c
 *
 * Copyright (C) 2025 MostlyK
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

