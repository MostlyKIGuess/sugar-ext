#include <gtk/gtk.h>
#include <math.h>
#include "../src/controllers/sugar-event-controllers.h"

typedef struct _TestController TestController;
typedef struct _TestControllerClass TestControllerClass;

static GtkWidget *status_label;
static GtkWidget *state_label;
static GtkWidget *test_area;

/**
 * Simple test controller that extends SugarEventController
high * 
 * This demonstrates how to create a custom gesture controller by:
 * - Extending the base SugarEventController class
 * - Implementing virtual methods for event handling
 * - Managing controller state through the gesture lifecycle
 */

struct _TestController {
    SugarEventController parent_instance;
    
    /* Private state for tracking gesture progress */
    gboolean button_pressed;
    gint press_count;
    gdouble start_x, start_y;
};

struct _TestControllerClass {
    SugarEventControllerClass parent_class;
};

G_DEFINE_TYPE (TestController, test_controller, SUGAR_TYPE_EVENT_CONTROLLER)

/**
 * Handle incoming events and decide if we should process them
 * 
 * This is the core method where you implement your gesture recognition logic.
 * Return TRUE if the event is relevant to your gesture, FALSE otherwise.
 * 
 * @param controller: The event controller instance
 * @param event: The GTK4 event to process
 * @return: TRUE if event was handled, FALSE if it should be passed on
 */
static gboolean
test_controller_handle_event (SugarEventController *controller,
                              GdkEvent *event)
{
    TestController *self = (TestController *)controller;
    GdkEventType event_type = gdk_event_get_event_type (event);
    
    switch (event_type) {
        case GDK_BUTTON_PRESS:
            {
                /* User pressed a mouse button - start tracking */
                guint button = gdk_button_event_get_button (event);
                if (button == GDK_BUTTON_PRIMARY) {  /* Left click only */
                    gdk_event_get_position (event, &self->start_x, &self->start_y);
                    self->button_pressed = TRUE;
                    self->press_count++;
                    
                    g_print ("Button pressed at (%.1f, %.1f) - press #%d\n", 
                            self->start_x, self->start_y, self->press_count);
                    
                    gtk_label_set_text (GTK_LABEL (status_label), 
                                       "Collecting gesture...");
                    
                    return TRUE;
                }
            }
            break;
            
        case GDK_BUTTON_RELEASE:
            {
                /* User released mouse button - gesture complete */
                guint button = gdk_button_event_get_button (event);
                if (button == GDK_BUTTON_PRIMARY && self->button_pressed) {
                    gdouble end_x, end_y;
                    gdouble distance;
                    
                    gdk_event_get_position (event, &end_x, &end_y);
                    
                    distance = sqrt (pow (end_x - self->start_x, 2) + 
                                   pow (end_y - self->start_y, 2));
                    
                    g_print ("Button released at (%.1f, %.1f) - moved %.1f pixels\n",
                            end_x, end_y, distance);
                    
                    self->button_pressed = FALSE;
                    
                    gtk_label_set_text (GTK_LABEL (status_label), 
                                       "Gesture recognized!");
                    
                    return TRUE;
                }
            }
            break;
            
        case GDK_MOTION_NOTIFY:
            /* Track mouse movement during gesture */
            if (self->button_pressed) {
                gdouble x, y;
                gdk_event_get_position (event, &x, &y);
                
                /* Could add logic here to cancel gesture if movement is too large */
                g_print ("Mouse moved to (%.1f, %.1f) during gesture\n", x, y);
                return TRUE;
            }
            break;
            
        default:
            /* We don't care about other event types */
            break;
    }
    
    return FALSE; 
}

/**
 * Report the current state of our gesture recognition
 * 
 * This method tells the Sugar toolkit what state our controller is in:
 * - NONE: No gesture in progress
 * - COLLECTING: We're tracking input but haven't recognized a gesture yet
 * - RECOGNIZED: We successfully recognized a complete gesture
 * - NOT_RECOGNIZED: We tried to recognize a gesture but it failed
 */
static SugarEventControllerState
test_controller_get_state (SugarEventController *controller)
{
    TestController *self = (TestController *)controller;
    
    if (self->button_pressed) {
        /* Currently tracking a button press */
        return SUGAR_EVENT_CONTROLLER_STATE_COLLECTING;
    } else if (self->press_count > 0) {
        /* We've completed at least one gesture */
        return SUGAR_EVENT_CONTROLLER_STATE_RECOGNIZED;
    } else {
        /* No activity yet */
        return SUGAR_EVENT_CONTROLLER_STATE_NONE;
    }
}

/**
 * Reset the controller to its initial state
 * 
 * This is called when the gesture should be cancelled or when starting fresh.
 * Clean up any internal state and prepare for the next gesture.
 */
static void
test_controller_reset (SugarEventController *controller)
{
    TestController *self = (TestController *)controller;
    
    g_print ("Controller reset - clearing all state\n");
    
    /* Reset all internal tracking state */
    self->button_pressed = FALSE;
    self->press_count = 0;
    self->start_x = self->start_y = 0.0;
    
    /* Update UI to show we're ready for new input */
    gtk_label_set_text (GTK_LABEL (status_label), "Ready for input");
}

/**
 * Initialize the test controller class
 * 
 * This sets up the virtual method table, connecting our implementations
 * to the base class interface.
 */
static void
test_controller_class_init (TestControllerClass *klass)
{
    SugarEventControllerClass *controller_class = SUGAR_EVENT_CONTROLLER_CLASS (klass);
    
    /* Connect our implementations to the virtual methods */
    controller_class->handle_event = test_controller_handle_event;
    controller_class->get_state = test_controller_get_state;
    controller_class->reset = test_controller_reset;
}

/**
 * Initialize a new test controller instance
 */
static void
test_controller_init (TestController *self)
{
    /* Set initial state */
    self->button_pressed = FALSE;
    self->press_count = 0;
    self->start_x = self->start_y = 0.0;
}

/**
 * Factory function to create a new test controller
 */
static SugarEventController *
test_controller_new (void)
{
    return g_object_new (test_controller_get_type (), NULL);
}

/**
 * Update the UI with current controller state information
 * 
 * This is called periodically to show users what the controller is doing.
 */
static gboolean
update_state_display (gpointer user_data)
{
    SugarEventController *controller = SUGAR_EVENT_CONTROLLER (user_data);
    SugarEventControllerState state;
    const gchar *state_text;
    gchar *display_text;
    
    /* Get the current state from our controller */
    state = sugar_event_controller_get_state (controller);
    
    /* Convert state enum to human-readable text */
    switch (state) {
        case SUGAR_EVENT_CONTROLLER_STATE_NONE:
            state_text = "NONE (no activity)";
            break;
        case SUGAR_EVENT_CONTROLLER_STATE_COLLECTING:
            state_text = "COLLECTING (tracking input)";
            break;
        case SUGAR_EVENT_CONTROLLER_STATE_RECOGNIZED:
            state_text = "RECOGNIZED (gesture complete)";
            break;
        case SUGAR_EVENT_CONTROLLER_STATE_NOT_RECOGNIZED:
            state_text = "NOT_RECOGNIZED (gesture failed)";
            break;
        default:
            state_text = "UNKNOWN";
            break;
    }
    
    display_text = g_strdup_printf ("Controller State: %s", state_text);
    gtk_label_set_text (GTK_LABEL (state_label), display_text);
    g_free (display_text);
    
    return G_SOURCE_CONTINUE;
}

/**
 * Signal handler for the controller's "began" signal
 * 
 * This demonstrates how to listen for controller lifecycle events.
 */
static void
on_controller_began (SugarEventController *controller, gpointer user_data)
{
    g_print ("Controller began signal fired!\n");
    gtk_label_set_text (GTK_LABEL (status_label), "Gesture began!");
}

/**
 * Signal handler for the controller's "ended" signal
 */
static void
on_controller_ended (SugarEventController *controller, gpointer user_data)
{
    g_print ("Controller ended signal fired!\n");
    gtk_label_set_text (GTK_LABEL (status_label), "Gesture ended!");
}

/**
 * Create the main application window with test interface
 */
static GtkWidget *
create_test_window (void)
{
    GtkWidget *window;
    GtkWidget *vbox;
    GtkWidget *instructions;
    GtkWidget *reset_button;
    SugarEventController *controller;
    
    /* Creates window */
    window = gtk_window_new ();
    gtk_window_set_title (GTK_WINDOW (window), "Sugar Event Controller Example");
    gtk_window_set_default_size (GTK_WINDOW (window), 500, 400);
    
    /* Creates layout */
    vbox = gtk_box_new (GTK_ORIENTATION_VERTICAL, 10);
    gtk_widget_set_margin_start (vbox, 20);
    gtk_widget_set_margin_end (vbox, 20);
    gtk_widget_set_margin_top (vbox, 20);
    gtk_widget_set_margin_bottom (vbox, 20);
    gtk_window_set_child (GTK_WINDOW (window), vbox);
    
    /* user instructions */
    instructions = gtk_label_new (
        "Event Controller Demo\n\n"
        "Instructions:\n"
        "• Click and drag in the test area below\n"
        "• Watch the status and state information update\n"
        "• Use the reset button to clear controller state\n"
        "• Check the terminal for detailed event logs"
    );
    gtk_label_set_justify (GTK_LABEL (instructions), GTK_JUSTIFY_CENTER);
    gtk_box_append (GTK_BOX (vbox), instructions);
    
    /* Creates test area  */
    test_area = gtk_button_new_with_label ("Test Area - Click and Drag Here!");
    gtk_widget_set_size_request (test_area, 300, 150);
    gtk_widget_set_halign (test_area, GTK_ALIGN_CENTER);
    gtk_box_append (GTK_BOX (vbox), test_area);
    
    /* Status labels */
    status_label = gtk_label_new ("Ready for input");
    gtk_widget_set_halign (status_label, GTK_ALIGN_CENTER);
    gtk_box_append (GTK_BOX (vbox), status_label);
    
    state_label = gtk_label_new ("Controller State: NONE");
    gtk_widget_set_halign (state_label, GTK_ALIGN_CENTER);
    gtk_box_append (GTK_BOX (vbox), state_label);
    
    reset_button = gtk_button_new_with_label ("Reset Controller");
    gtk_widget_set_halign (reset_button, GTK_ALIGN_CENTER);
    gtk_box_append (GTK_BOX (vbox), reset_button);
    
    /* === CONTROLLER SETUP === */
    
    controller = test_controller_new ();
    
    /* Connect to controller lifecycle signals for demonstration */
    g_signal_connect (controller, "began", G_CALLBACK (on_controller_began), NULL);
    g_signal_connect (controller, "ended", G_CALLBACK (on_controller_ended), NULL);
    
    /* Attach controller to the test area so it receives events */
    sugar_event_controller_attach (controller, test_area, 
                                  SUGAR_EVENT_CONTROLLER_FLAG_NONE);
    
    /* Connect reset button to demonstrate programmatic reset */
    g_signal_connect_swapped (reset_button, "clicked",
                             G_CALLBACK (sugar_event_controller_reset), controller);
    
    /* Start periodic UI updates to show controller state changes */
    g_timeout_add (100, update_state_display, controller);
    
    return window;
}

/**
 * Application activation callback
 */
static void
on_app_activate (GtkApplication *app, gpointer user_data)
{
    GtkWidget *window = create_test_window ();
    gtk_application_add_window (app, GTK_WINDOW (window));
    gtk_window_present (GTK_WINDOW (window));
}

/**
 * Main entry point
 * 
 * This example demonstrates:
 * 1. How to create a custom SugarEventController subclass
 * 2. How to implement event handling logic
 * 3. How to manage controller state
 * 4. How to attach controllers to widgets
 * 5. How to listen for controller signals
 * 6. How to reset controllers programmatically
 */
int
main (int argc, char *argv[])
{
    GtkApplication *app;
    int status;
    
    app = gtk_application_new ("org.sugarlabs.EventControllerExample",
                              G_APPLICATION_DEFAULT_FLAGS);
    
    g_signal_connect (app, "activate", G_CALLBACK (on_app_activate), NULL);
    
    status = g_application_run (G_APPLICATION (app), argc, argv);
    g_object_unref (app);
    
    return status;
}
