#include <stdio.h>
#include <sugar-ext.h>
#include <glib.h>
#include <gio/gio.h>
#include <string.h>
#include <unistd.h>

static void print_file_attributes(GFile *file) {
    printf("\nFile Attributes:\n");
    printf("================\n");
    
    gchar *path = g_file_get_path(file);
    printf("File: %s\n\n", path);
    
    // Get full attributes
    SugarFileAttributes *attrs = sugar_file_attributes_get_from_file(file, NULL);
    if (!attrs) {
        printf("No Sugar attributes found or not supported.\n");
        g_free(path);
        return;
    }
    
    printf("Title: %s\n", attrs->title ? attrs->title : "(none)");
    printf("Description: %s\n", attrs->description ? attrs->description : "(none)");
    printf("Tags: %s\n", attrs->tags ? attrs->tags : "(none)");
    printf("Created by: %s\n", attrs->activity ? attrs->activity : "(unknown)");
    
    if (attrs->creation_time > 0) {
        GDateTime *dt = g_date_time_new_from_unix_local(attrs->creation_time / G_USEC_PER_SEC);
        gchar *time_str = g_date_time_format(dt, "%Y-%m-%d %H:%M:%S");
        printf("Created: %s\n", time_str);
        g_free(time_str);
        g_date_time_unref(dt);
    }
    
    if (attrs->modification_time > 0) {
        GDateTime *dt = g_date_time_new_from_unix_local(attrs->modification_time / G_USEC_PER_SEC);
        gchar *time_str = g_date_time_format(dt, "%Y-%m-%d %H:%M:%S");
        printf("Modified: %s\n", time_str);
        g_free(time_str);
        g_date_time_unref(dt);
    }
    
    printf("Preview: %s\n", attrs->preview_path ? attrs->preview_path : "(none)");
    
    sugar_file_attributes_free(attrs);
    g_free(path);
}

int main(int argc, char *argv[]) {
    printf("Sugar File Attributes Example\n");
    printf("=============================\n");

    // Create a temporary file for demonstration
    gchar *temp_path = NULL;
    GError *error = NULL;
    gint fd = g_file_open_tmp("sugar_demo_XXXXXX.txt", &temp_path, &error);
    
    if (fd == -1) {
        printf("Failed to create temporary file: %s\n", error->message);
        g_error_free(error);
        return 1;
    }
    
    // Write some content to the file
    const gchar *content = "This is a sample Sugar activity file.\nIt demonstrates file attribute management.";
    write(fd, content, strlen(content));
    close(fd);
    
    GFile *file = g_file_new_for_path(temp_path);
    printf("Created temporary file: %s\n", temp_path);

    // Test 1: Basic attribute setting
    printf("\n1. Setting basic attributes...\n");
    
    gboolean success = sugar_file_attributes_set_title(file, "My Drawing");
    if (!success) {
        printf("Extended attributes not supported on this filesystem.\n");
        printf("This is normal for some filesystems (tmpfs, etc.)\n");
        g_object_unref(file);
        unlink(temp_path);
        g_free(temp_path);
        return 0;
    }
    
    sugar_file_attributes_set_description(file, "A beautiful drawing I made");
    
    const gchar *tags[] = {"art", "drawing", "creative", NULL};
    sugar_file_attributes_set_tags(file, tags);
    
    sugar_file_attributes_mark_as_created_by(file, "Paint");
    
    print_file_attributes(file);

    // Test 2: Full structure approach
    printf("\n2. Using full attribute structure...\n");
    
    SugarFileAttributes *attrs = sugar_file_attributes_new();
    attrs->title = g_strdup("Updated Drawing");
    attrs->description = g_strdup("An updated version with more details");
    attrs->tags = g_strdup("art,drawing,creative,updated");
    attrs->activity = g_strdup("Paint");
    attrs->preview_path = g_strdup("/tmp/preview.png");
    
    success = sugar_file_attributes_save_to_file(attrs, file, &error);
    if (!success) {
        printf("Failed to save attributes: %s\n", error->message);
        g_error_free(error);
    }
    
    sugar_file_attributes_free(attrs);
    print_file_attributes(file);

    // Test 3: Individual attribute access
    printf("\n3. Individual attribute access...\n");
    
    gchar *title = sugar_file_attributes_get_title(file);
    gchar *description = sugar_file_attributes_get_description(file);
    gchar **file_tags = sugar_file_attributes_get_tags(file);
    
    printf("Retrieved title: %s\n", title);
    printf("Retrieved description: %s\n", description);
    printf("Retrieved tags: ");
    if (file_tags) {
        for (int i = 0; file_tags[i]; i++) {
            printf("%s%s", i > 0 ? ", " : "", file_tags[i]);
        }
        printf("\n");
        g_strfreev(file_tags);
    } else {
        printf("(none)\n");
    }
    
    g_free(title);
    g_free(description);

    // Test 4: Activity workflow simulation
    printf("\n4. Simulating activity workflow...\n");
    
    // simulate lets say like story writing
    sugar_file_attributes_set_title(file, "My Story");
    sugar_file_attributes_set_description(file, "A story about adventures");
    sugar_file_attributes_mark_as_created_by(file, "Write");
    
    const gchar *story_tags[] = {"story", "writing", "adventure", NULL};
    sugar_file_attributes_set_tags(file, story_tags);
    
    printf("Marked file as created by Write activity.\n");
    print_file_attributes(file);

    g_object_unref(file);
    unlink(temp_path);
    g_free(temp_path);

    printf("\nExample completed successfully!\n");
    printf("Note: Attributes are stored as extended attributes (xattr)\n");
    printf("and may not be visible in standard file managers.\n");
    
    return 0;
}
