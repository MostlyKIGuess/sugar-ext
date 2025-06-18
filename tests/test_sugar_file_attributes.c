#include <glib.h>
#include <sugar-ext.h>
#include <gio/gio.h>
#include <sys/stat.h>

static void test_file_attributes_creation(void) {
    SugarFileAttributes *attrs = sugar_file_attributes_new();
    g_assert_nonnull(attrs);
    
    // Check initial state
    g_assert_cmpint(attrs->creation_time, >, 0);
    g_assert_cmpint(attrs->modification_time, >, 0);
    
    sugar_file_attributes_free(attrs);
}

static void test_file_attributes_basic_operations(void) {
    // Create a temporary file
    gchar *temp_path = NULL;
    gint fd = g_file_open_tmp("sugar_test_XXXXXX", &temp_path, NULL);
    if (fd == -1) {
        g_test_skip("Could not create temporary file");
        return;
    }
    close(fd);
    
    GFile *file = g_file_new_for_path(temp_path);
    
    // Test setting title
    gboolean success = sugar_file_attributes_set_title(file, "Test Document");
    if (!success) {
        g_test_skip("Extended attributes not supported on this filesystem");
        g_object_unref(file);
        unlink(temp_path);
        g_free(temp_path);
        return;
    }
    
    // Test getting title
    gchar *title = sugar_file_attributes_get_title(file);
    g_assert_cmpstr(title, ==, "Test Document");
    g_free(title);
    
    // Test setting description
    success = sugar_file_attributes_set_description(file, "A test document for Sugar");
    g_assert_true(success);
    
    gchar *description = sugar_file_attributes_get_description(file);
    g_assert_cmpstr(description, ==, "A test document for Sugar");
    g_free(description);
    
    // Test setting tags
    const gchar *tags[] = {"test", "document", "sugar", NULL};
    success = sugar_file_attributes_set_tags(file, tags);
    g_assert_true(success);
    
    gchar **retrieved_tags = sugar_file_attributes_get_tags(file);
    g_assert_nonnull(retrieved_tags);
    g_assert_cmpint(g_strv_length(retrieved_tags), ==, 3);
    g_strfreev(retrieved_tags);
    
    // Clean up
    g_object_unref(file);
    unlink(temp_path);
    g_free(temp_path);
}

static void test_file_attributes_full_structure(void) {
    // Create a temporary file
    gchar *temp_path = NULL;
    gint fd = g_file_open_tmp("sugar_test_XXXXXX", &temp_path, NULL);
    if (fd == -1) {
        g_test_skip("Could not create temporary file");
        return;
    }
    close(fd);
    
    GFile *file = g_file_new_for_path(temp_path);
    
    // Create attributes structure
    SugarFileAttributes *attrs = sugar_file_attributes_new();
    attrs->title = g_strdup("Full Test Document");
    attrs->description = g_strdup("A comprehensive test document");
    attrs->tags = g_strdup("test,full,comprehensive");
    attrs->activity = g_strdup("Write");
    
    // Save to file
    GError *error = NULL;
    gboolean success = sugar_file_attributes_save_to_file(attrs, file, &error);
    
    if (!success) {
        if (error && error->code == G_IO_ERROR_NOT_SUPPORTED) {
            g_test_skip("Extended attributes not supported on this filesystem");
        } else {
            g_test_skip("Could not save attributes to file");
        }
        g_clear_error(&error);
        sugar_file_attributes_free(attrs);
        g_object_unref(file);
        unlink(temp_path);
        g_free(temp_path);
        return;
    }
    
    // Load from file
    SugarFileAttributes *loaded = sugar_file_attributes_get_from_file(file, &error);
    g_assert_no_error(error);
    g_assert_nonnull(loaded);
    
    g_assert_cmpstr(loaded->title, ==, "Full Test Document");
    g_assert_cmpstr(loaded->description, ==, "A comprehensive test document");
    g_assert_cmpstr(loaded->tags, ==, "test,full,comprehensive");
    g_assert_cmpstr(loaded->activity, ==, "Write");
    
    // Clean up
    sugar_file_attributes_free(attrs);
    sugar_file_attributes_free(loaded);
    g_object_unref(file);
    unlink(temp_path);
    g_free(temp_path);
}

static void test_activity_marking(void) {
    // Create a temporary file
    gchar *temp_path = NULL;
    gint fd = g_file_open_tmp("sugar_test_XXXXXX", &temp_path, NULL);
    if (fd == -1) {
        g_test_skip("Could not create temporary file");
        return;
    }
    close(fd);
    
    GFile *file = g_file_new_for_path(temp_path);
    
    // Mark as created by activity
    gboolean success = sugar_file_attributes_mark_as_created_by(file, "Paint");
    if (!success) {
        g_test_skip("Extended attributes not supported on this filesystem");
        g_object_unref(file);
        unlink(temp_path);
        g_free(temp_path);
        return;
    }
    
    // Verify activity was set
    SugarFileAttributes *attrs = sugar_file_attributes_get_from_file(file, NULL);
    g_assert_nonnull(attrs);
    g_assert_cmpstr(attrs->activity, ==, "Paint");
    g_assert_cmpint(attrs->creation_time, >, 0);
    
    sugar_file_attributes_free(attrs);
    g_object_unref(file);
    unlink(temp_path);
    g_free(temp_path);
}

int main(int argc, char *argv[]) {
    g_test_init(&argc, &argv, NULL);

    g_test_add_func("/sugar/file-attributes/creation", test_file_attributes_creation);
    g_test_add_func("/sugar/file-attributes/basic-operations", test_file_attributes_basic_operations);
    g_test_add_func("/sugar/file-attributes/full-structure", test_file_attributes_full_structure);
    g_test_add_func("/sugar/file-attributes/activity-marking", test_activity_marking);

    return g_test_run();
}
