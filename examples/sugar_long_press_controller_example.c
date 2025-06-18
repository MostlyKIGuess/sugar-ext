#include <gtk/gtk.h>
#include "../src/controllers/sugar-event-controllers.h"

static void
on_long_press (SugarLongPressController *controller,
               gint                      x,
               gint                      y,
               gpointer                  user_data)
{
  GtkWidget *label = GTK_WIDGET (user_data);
  gchar *text;
  
  text = g_strdup_printf ("Long press detected at (%d, %d)", x, y);
  gtk_label_set_text (GTK_LABEL (label), text);
  g_free (text);
  
  g_print ("Long press at coordinates: %d, %d\n", x, y);
}

static void
on_activate (GtkApplication *app,
             gpointer        user_data)
{
  GtkWidget *window;
  GtkWidget *box;
  GtkWidget *label;
  GtkWidget *button;
  
  /* Creates window */
  window = gtk_application_window_new (app);
  gtk_window_set_title (GTK_WINDOW (window), "Sugar Long Press Controller Example");
  gtk_window_set_default_size (GTK_WINDOW (window), 400, 300);
  
  /* Creates layout */
  box = gtk_box_new (GTK_ORIENTATION_VERTICAL, 10);
  gtk_widget_set_margin_top (box, 20);
  gtk_widget_set_margin_bottom (box, 20);
  gtk_widget_set_margin_start (box, 20);
  gtk_widget_set_margin_end (box, 20);
  gtk_window_set_child (GTK_WINDOW (window), box);
  
  /* Creates label */
  label = gtk_label_new ("Long press on the button below\n(hold for 500ms or more)");
  gtk_label_set_justify (GTK_LABEL (label), GTK_JUSTIFY_CENTER);
  gtk_box_append (GTK_BOX (box), label);
  
  /* Creates status  */
  label = gtk_label_new ("No long press detected yet");
  gtk_label_set_justify (GTK_LABEL (label), GTK_JUSTIFY_CENTER);
  gtk_box_append (GTK_BOX (box), label);
  
  /* Test Area */
  button = gtk_button_new_with_label ("Long Press Me!");
  gtk_widget_set_size_request (button, 200, 100);
  gtk_widget_set_halign (button, GTK_ALIGN_CENTER);
  gtk_box_append (GTK_BOX (box), button);
  
  /* Create and configure long press controller */
  // Create the long-press controller
  SugarEventController *controller = sugar_long_press_controller_new();

  // Attach the controller to the button.
  // NOTE: We use SUGAR_EVENT_CONTROLLER_FLAG_EMULATE_TOUCH to allow the
  // long-press to be triggered by a mouse click-and-hold as well as a
  // touch event. I don't have a touch device to test with, but this
  // should work on touch devices as well.
  sugar_event_controller_attach(controller, button, SUGAR_EVENT_CONTROLLER_FLAG_EMULATE_TOUCH);

  // Connect to the "pressed" signal
  g_signal_connect(controller, "pressed", G_CALLBACK(on_long_press), label);
  
  gtk_window_present (GTK_WINDOW (window));
}

int
main (int argc, char *argv[])
{
  GtkApplication *app;
  int status;
  
  app = gtk_application_new ("org.sugarlabs.LongPressExample",
                             G_APPLICATION_DEFAULT_FLAGS);
  g_signal_connect (app, "activate", G_CALLBACK (on_activate), NULL);
  
  status = g_application_run (G_APPLICATION (app), argc, argv);
  g_object_unref (app);
  
  return status;
}
