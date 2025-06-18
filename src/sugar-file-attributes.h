/*
 * Copyright (C) 2025 MostlyK
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the
 * Free Software Foundation, Inc., 59 Temple Place - Suite 330,
 * Boston, MA 02111-1307, USA.
 */

#ifndef __SUGAR_FILE_ATTRIBUTES_H__
#define __SUGAR_FILE_ATTRIBUTES_H__

#include <glib-object.h>
#include <gio/gio.h>

G_BEGIN_DECLS

/**
 * SugarFileAttributes:
 * @title: Display title for the file
 * @description: Description or summary
 * @tags: Comma-separated tags for categorization
 * @activity: Sugar activity that created this file
 * @creation_time: When the file was created (timestamp)
 * @modification_time: When the file was last modified (timestamp)
 * @preview_path: Path to preview/thumbnail image
 *
 * Extended attributes for Sugar activity files.
 * @gtype-name SugarFileAttributes
 */
typedef struct {
    gchar *title;
    gchar *description;
    gchar *tags;
    gchar *activity;
    gint64 creation_time;
    gint64 modification_time;
    gchar *preview_path;
} SugarFileAttributes;

GType sugar_file_attributes_get_type (void);

/* File attributes API */
SugarFileAttributes* sugar_file_attributes_new              (void);
void                 sugar_file_attributes_free            (SugarFileAttributes *attrs);

/* Reading attributes */
SugarFileAttributes* sugar_file_attributes_get_from_file   (GFile *file, GError **error);
gboolean             sugar_file_attributes_load_from_file  (SugarFileAttributes *attrs, GFile *file, GError **error);

/* Writing attributes */
gboolean             sugar_file_attributes_save_to_file    (SugarFileAttributes *attrs, GFile *file, GError **error);

/* Convenience functions */
gchar*               sugar_file_attributes_get_title       (GFile *file);
gboolean             sugar_file_attributes_set_title       (GFile *file, const gchar *title);
gchar*               sugar_file_attributes_get_description (GFile *file);
gboolean             sugar_file_attributes_set_description (GFile *file, const gchar *description);
gchar**              sugar_file_attributes_get_tags        (GFile *file);
gboolean             sugar_file_attributes_set_tags        (GFile *file, const gchar * const *tags);

/* Activity integration */
gboolean             sugar_file_attributes_mark_as_created_by (GFile *file, const gchar *activity_name);

G_END_DECLS

#endif /* __SUGAR_FILE_ATTRIBUTES_H__ */
