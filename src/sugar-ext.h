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
#include "sugar-grid.h"
#include "sugar-file-attributes.h"
#include "controllers/sugar-event-controllers.h"

G_BEGIN_DECLS

/* Utility functions */
gdouble sugar_get_grid_cell_size(void);
gdouble sugar_scale_for_touch(gdouble size);
gboolean sugar_is_touch_device(void);

G_END_DECLS
