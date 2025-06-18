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

#include "sugar-file-attributes.h"
#include <string.h>
#include <sys/xattr.h>

#define SUGAR_XATTR_PREFIX "user.sugar."
#define SUGAR_XATTR_TITLE SUGAR_XATTR_PREFIX "title"
#define SUGAR_XATTR_DESCRIPTION SUGAR_XATTR_PREFIX "description"
#define SUGAR_XATTR_TAGS SUGAR_XATTR_PREFIX "tags"
#define SUGAR_XATTR_ACTIVITY SUGAR_XATTR_PREFIX "activity"
#define SUGAR_XATTR_CREATION_TIME SUGAR_XATTR_PREFIX "creation_time"
#define SUGAR_XATTR_MODIFICATION_TIME SUGAR_XATTR_PREFIX "modification_time"
#define SUGAR_XATTR_PREVIEW_PATH SUGAR_XATTR_PREFIX "preview_path"

/**
 * sugar_file_attributes_new:
 *
 * Creates a new #SugarFileAttributes structure.
 *
 * Returns: (transfer full): A newly allocated #SugarFileAttributes. Free with sugar_file_attributes_free().
 */
SugarFileAttributes*
sugar_file_attributes_new(void)
{
    SugarFileAttributes *attrs = g_new0(SugarFileAttributes, 1);
    
    attrs->creation_time = g_get_real_time();
    attrs->modification_time = attrs->creation_time;
    
    return attrs;
}

/**
 * sugar_file_attributes_free:
 * @attrs: A #SugarFileAttributes
 *
 * Frees a #SugarFileAttributes structure.
 */
void
sugar_file_attributes_free(SugarFileAttributes *attrs)
{
    if (!attrs) return;
    
    g_free(attrs->title);
    g_free(attrs->description);
    g_free(attrs->tags);
    g_free(attrs->activity);
    g_free(attrs->preview_path);
    g_free(attrs);
}

static gchar*
get_xattr_string(const gchar *path, const gchar *name)
{
    ssize_t size = getxattr(path, name, NULL, 0);
    if (size <= 0) return NULL;
    
    gchar *buffer = g_malloc(size + 1);
    ssize_t result = getxattr(path, name, buffer, size);
    
    if (result <= 0) {
        g_free(buffer);
        return NULL;
    }
    
    buffer[result] = '\0';
    return buffer;
}

static gboolean
set_xattr_string(const gchar *path, const gchar *name, const gchar *value)
{
    if (!value) {
        removexattr(path, name);
        return TRUE;
    }
    
    return setxattr(path, name, value, strlen(value), 0) == 0;
}

static gint64
get_xattr_int64(const gchar *path, const gchar *name)
{
    gchar *str = get_xattr_string(path, name);
    if (!str) return 0;
    
    gint64 value = g_ascii_strtoll(str, NULL, 10);
    g_free(str);
    return value;
}

static gboolean
set_xattr_int64(const gchar *path, const gchar *name, gint64 value)
{
    gchar *str = g_strdup_printf("%" G_GINT64_FORMAT, value);
    gboolean result = set_xattr_string(path, name, str);
    g_free(str);
    return result;
}

/**
 * sugar_file_attributes_get_from_file:
 * @file: A #GFile
 * @error: Return location for error
 *
 * Reads Sugar file attributes from the given file.
 *
 * Returns: (transfer full): A #SugarFileAttributes or %NULL on error
 */
SugarFileAttributes*
sugar_file_attributes_get_from_file(GFile *file, GError **error)
{
    g_return_val_if_fail(G_IS_FILE(file), NULL);
    
    gchar *path = g_file_get_path(file);
    if (!path) {
        g_set_error(error, G_IO_ERROR, G_IO_ERROR_NOT_SUPPORTED,
                   "File does not support extended attributes");
        return NULL;
    }
    
    SugarFileAttributes *attrs = sugar_file_attributes_new();
    
    attrs->title = get_xattr_string(path, SUGAR_XATTR_TITLE);
    attrs->description = get_xattr_string(path, SUGAR_XATTR_DESCRIPTION);
    attrs->tags = get_xattr_string(path, SUGAR_XATTR_TAGS);
    attrs->activity = get_xattr_string(path, SUGAR_XATTR_ACTIVITY);
    attrs->preview_path = get_xattr_string(path, SUGAR_XATTR_PREVIEW_PATH);
    
    attrs->creation_time = get_xattr_int64(path, SUGAR_XATTR_CREATION_TIME);
    attrs->modification_time = get_xattr_int64(path, SUGAR_XATTR_MODIFICATION_TIME);
    
    // Fallback to file system times if not set
    if (attrs->creation_time == 0 || attrs->modification_time == 0) {
        GFileInfo *info = g_file_query_info(file, 
                                           G_FILE_ATTRIBUTE_TIME_CREATED ","
                                           G_FILE_ATTRIBUTE_TIME_MODIFIED,
                                           G_FILE_QUERY_INFO_NONE, NULL, NULL);
        if (info) {
            if (attrs->creation_time == 0) {
                attrs->creation_time = g_file_info_get_attribute_uint64(info, G_FILE_ATTRIBUTE_TIME_CREATED);
            }
            if (attrs->modification_time == 0) {
                attrs->modification_time = g_file_info_get_attribute_uint64(info, G_FILE_ATTRIBUTE_TIME_MODIFIED);
            }
            g_object_unref(info);
        }
    }
    
    g_free(path);
    return attrs;
}

/**
 * sugar_file_attributes_load_from_file:
 * @attrs: A #SugarFileAttributes
 * @file: A #GFile
 * @error: Return location for error
 *
 * Loads Sugar file attributes from the given file into the structure.
 *
 * Returns: %TRUE on success, %FALSE on error
 */
gboolean
sugar_file_attributes_load_from_file(SugarFileAttributes *attrs, GFile *file, GError **error)
{
    g_return_val_if_fail(attrs != NULL, FALSE);
    g_return_val_if_fail(G_IS_FILE(file), FALSE);
    
    SugarFileAttributes *temp = sugar_file_attributes_get_from_file(file, error);
    if (!temp) return FALSE;
    
    // Copy values
    g_free(attrs->title);
    g_free(attrs->description);
    g_free(attrs->tags);
    g_free(attrs->activity);
    g_free(attrs->preview_path);
    
    attrs->title = g_strdup(temp->title);
    attrs->description = g_strdup(temp->description);
    attrs->tags = g_strdup(temp->tags);
    attrs->activity = g_strdup(temp->activity);
    attrs->preview_path = g_strdup(temp->preview_path);
    attrs->creation_time = temp->creation_time;
    attrs->modification_time = temp->modification_time;
    
    sugar_file_attributes_free(temp);
    return TRUE;
}

/**
 * sugar_file_attributes_save_to_file:
 * @attrs: A #SugarFileAttributes
 * @file: A #GFile
 * @error: Return location for error
 *
 * Saves Sugar file attributes to the given file.
 *
 * Returns: %TRUE on success, %FALSE on error
 */
gboolean
sugar_file_attributes_save_to_file(SugarFileAttributes *attrs, GFile *file, GError **error)
{
    g_return_val_if_fail(attrs != NULL, FALSE);
    g_return_val_if_fail(G_IS_FILE(file), FALSE);
    
    gchar *path = g_file_get_path(file);
    if (!path) {
        g_set_error(error, G_IO_ERROR, G_IO_ERROR_NOT_SUPPORTED,
                   "File does not support extended attributes");
        return FALSE;
    }
    
    gboolean success = TRUE;
    
    attrs->modification_time = g_get_real_time();
    
    success &= set_xattr_string(path, SUGAR_XATTR_TITLE, attrs->title);
    success &= set_xattr_string(path, SUGAR_XATTR_DESCRIPTION, attrs->description);
    success &= set_xattr_string(path, SUGAR_XATTR_TAGS, attrs->tags);
    success &= set_xattr_string(path, SUGAR_XATTR_ACTIVITY, attrs->activity);
    success &= set_xattr_string(path, SUGAR_XATTR_PREVIEW_PATH, attrs->preview_path);
    success &= set_xattr_int64(path, SUGAR_XATTR_CREATION_TIME, attrs->creation_time);
    success &= set_xattr_int64(path, SUGAR_XATTR_MODIFICATION_TIME, attrs->modification_time);
    
    if (!success) {
        g_set_error(error, G_IO_ERROR, G_IO_ERROR_FAILED,
                   "Failed to write extended attributes");
    }
    
    g_free(path);
    return success;
}

/**
 * sugar_file_attributes_get_title:
 * @file: A #GFile
 *
 * Gets the title attribute for a file.
 *
 * Returns: (transfer full): The title string or %NULL
 */
gchar*
sugar_file_attributes_get_title(GFile *file)
{
    g_return_val_if_fail(G_IS_FILE(file), NULL);
    
    SugarFileAttributes *attrs = sugar_file_attributes_get_from_file(file, NULL);
    if (!attrs) return NULL;
    
    gchar *title = g_strdup(attrs->title);
    sugar_file_attributes_free(attrs);
    return title;
}

/**
 * sugar_file_attributes_set_title:
 * @file: A #GFile
 * @title: The title to set
 *
 * Sets the title attribute for a file.
 *
 * Returns: %TRUE on success, %FALSE on error
 */
gboolean
sugar_file_attributes_set_title(GFile *file, const gchar *title)
{
    g_return_val_if_fail(G_IS_FILE(file), FALSE);
    
    gchar *path = g_file_get_path(file);
    if (!path) return FALSE;
    
    gboolean result = set_xattr_string(path, SUGAR_XATTR_TITLE, title);
    g_free(path);
    return result;
}

/**
 * sugar_file_attributes_get_description:
 * @file: A #GFile
 *
 * Gets the description attribute for a file.
 *
 * Returns: (transfer full): The description string or %NULL
 */
gchar*
sugar_file_attributes_get_description(GFile *file)
{
    g_return_val_if_fail(G_IS_FILE(file), NULL);
    
    SugarFileAttributes *attrs = sugar_file_attributes_get_from_file(file, NULL);
    if (!attrs) return NULL;
    
    gchar *description = g_strdup(attrs->description);
    sugar_file_attributes_free(attrs);
    return description;
}

/**
 * sugar_file_attributes_set_description:
 * @file: A #GFile
 * @description: The description to set
 *
 * Sets the description attribute for a file.
 *
 * Returns: %TRUE on success, %FALSE on error
 */
gboolean
sugar_file_attributes_set_description(GFile *file, const gchar *description)
{
    g_return_val_if_fail(G_IS_FILE(file), FALSE);
    
    gchar *path = g_file_get_path(file);
    if (!path) return FALSE;
    
    gboolean result = set_xattr_string(path, SUGAR_XATTR_DESCRIPTION, description);
    g_free(path);
    return result;
}

/**
 * sugar_file_attributes_get_tags:
 * @file: A #GFile
 *
 * Gets the tags attribute for a file.
 *
 * Returns: (transfer full): A NULL-terminated array of tag strings
 */
gchar**
sugar_file_attributes_get_tags(GFile *file)
{
    g_return_val_if_fail(G_IS_FILE(file), NULL);
    
    SugarFileAttributes *attrs = sugar_file_attributes_get_from_file(file, NULL);
    if (!attrs || !attrs->tags) {
        if (attrs) sugar_file_attributes_free(attrs);
        return NULL;
    }
    
    gchar **tags = g_strsplit(attrs->tags, ",", -1);
    sugar_file_attributes_free(attrs);
    
    // Trim whitespace from tags
    for (int i = 0; tags[i]; i++) {
        gchar *trimmed = g_strstrip(tags[i]);
        if (trimmed != tags[i]) {
            gchar *new_tag = g_strdup(trimmed);
            g_free(tags[i]);
            tags[i] = new_tag;
        }
    }
    
    return tags;
}

/**
 * sugar_file_attributes_set_tags:
 * @file: A #GFile
 * @tags: NULL-terminated array of tag strings
 *
 * Sets the tags attribute for a file.
 *
 * Returns: %TRUE on success, %FALSE on error
 */
gboolean
sugar_file_attributes_set_tags(GFile *file, const gchar * const *tags)
{
    g_return_val_if_fail(G_IS_FILE(file), FALSE);
    
    gchar *path = g_file_get_path(file);
    if (!path) return FALSE;
    
    gchar *tags_str = NULL;
    if (tags && tags[0]) {
        tags_str = g_strjoinv(",", (gchar**)tags);
    }
    
    gboolean result = set_xattr_string(path, SUGAR_XATTR_TAGS, tags_str);
    
    g_free(tags_str);
    g_free(path);
    return result;
}

/**
 * sugar_file_attributes_mark_as_created_by:
 * @file: A #GFile
 * @activity_name: Name of the Sugar activity
 *
 * Marks a file as created by a specific Sugar activity.
 *
 * Returns: %TRUE on success, %FALSE on error
 */
gboolean
sugar_file_attributes_mark_as_created_by(GFile *file, const gchar *activity_name)
{
    g_return_val_if_fail(G_IS_FILE(file), FALSE);
    g_return_val_if_fail(activity_name != NULL, FALSE);
    
    gchar *path = g_file_get_path(file);
    if (!path) return FALSE;
    
    gboolean result = set_xattr_string(path, SUGAR_XATTR_ACTIVITY, activity_name);
    
    // Also set creation time if not already set
    if (result && get_xattr_int64(path, SUGAR_XATTR_CREATION_TIME) == 0) {
        result &= set_xattr_int64(path, SUGAR_XATTR_CREATION_TIME, g_get_real_time());
    }
    
    g_free(path);
    return result;
}
