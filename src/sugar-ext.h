/* sugar-ext.h
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

#pragma once

#include <gtk/gtk.h>

G_BEGIN_DECLS

/**
 * SugarXOColors:
 * @fill_color: The fill color (RGBA)
 * @stroke_color: The stroke color (RGBA)
 *
 * Structure representing XO buddy colors used in Sugar.
 */
typedef struct {
    GdkRGBA fill_color;
    GdkRGBA stroke_color;
} SugarXOColors;

/* XO Colors API */
SugarXOColors* sugar_xo_colors_new(void);
SugarXOColors* sugar_xo_colors_new_from_string(const gchar *color_spec);
void sugar_xo_colors_free(SugarXOColors *colors);
gchar* sugar_xo_colors_to_string(SugarXOColors *colors);

/* Utility functions */
gdouble sugar_get_grid_cell_size(void);
gdouble sugar_scale_for_touch(gdouble size);
gboolean sugar_is_touch_device(void);

G_END_DECLS
