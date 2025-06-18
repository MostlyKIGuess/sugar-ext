/*
 * Copyright (C) 2012, One Laptop Per Child.
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
 *
 * Author(s): Carlos Garnacho <carlos@lanedo.com>
 */

#define SUGAR_TOOLKIT_COMPILATION
#include "sugar-long-press-controller.h"
#include <gtk/gtk.h>

struct _SugarLongPressControllerPrivate
{
  GdkDevice *device;
  GdkEventSequence *sequence;
  gint64 start_time;
  gint x;
  gint y;
  gint root_x;
  gint root_y;
  guint timeout_id;
  guint threshold;
  guint delay;
  guint cancelled : 1;
  guint triggered : 1;
};

G_DEFINE_TYPE_WITH_PRIVATE (SugarLongPressController,
                            sugar_long_press_controller,
                            SUGAR_TYPE_EVENT_CONTROLLER)

enum {
  PRESSED,
  N_SIGNALS
};

enum {
  PROP_0,
  PROP_DELAY_FACTOR
};

static guint signals[N_SIGNALS] = { 0 };

#define DEFAULT_LONG_PRESS_TIME 500
#define TOUCH_THRESHOLD 20

#define sugar_long_press_controller_get_instance_private(self) \
  ((SugarLongPressControllerPrivate *) \
   sugar_long_press_controller_get_instance_private (self))

static void
sugar_long_press_controller_finalize (GObject *object)
{
  SugarLongPressController *controller = SUGAR_LONG_PRESS_CONTROLLER (object);
  SugarLongPressControllerPrivate *priv = controller->priv;

  if (priv->timeout_id)
    {
      g_source_remove (priv->timeout_id);
      priv->timeout_id = 0;
    }

  G_OBJECT_CLASS (sugar_long_press_controller_parent_class)->finalize (object);
}

static void
sugar_long_press_controller_set_property (GObject      *object,
                                          guint         prop_id,
                                          const GValue *value,
                                          GParamSpec   *pspec)
{
  SugarLongPressController *controller = SUGAR_LONG_PRESS_CONTROLLER (object);
  SugarLongPressControllerPrivate *priv = controller->priv;

  switch (prop_id)
    {
    case PROP_DELAY_FACTOR:
      priv->delay = DEFAULT_LONG_PRESS_TIME * g_value_get_double (value);
      break;
    default:
      G_OBJECT_WARN_INVALID_PROPERTY_ID (object, prop_id, pspec);
    }
}

static void
sugar_long_press_controller_get_property (GObject    *object,
                                          guint       prop_id,
                                          GValue     *value,
                                          GParamSpec *pspec)
{
  SugarLongPressController *controller = SUGAR_LONG_PRESS_CONTROLLER (object);
  SugarLongPressControllerPrivate *priv = controller->priv;

  switch (prop_id)
    {
    case PROP_DELAY_FACTOR:
      g_value_set_double (value, (gdouble) priv->delay / DEFAULT_LONG_PRESS_TIME);
      break;
    default:
      G_OBJECT_WARN_INVALID_PROPERTY_ID (object, prop_id, pspec);
    }
}

static gboolean
sugar_long_press_controller_timeout (gpointer user_data)
{
  SugarLongPressController *controller = user_data;
  SugarLongPressControllerPrivate *priv = controller->priv;

  priv->timeout_id = 0;

  if (!priv->cancelled)
    {
      priv->triggered = TRUE;
      g_signal_emit (controller, signals[PRESSED], 0, priv->x, priv->y);
    }

  return G_SOURCE_REMOVE;
}

static void
sugar_long_press_controller_reset (SugarLongPressController *controller)
{
  SugarLongPressControllerPrivate *priv = controller->priv;

  if (priv->timeout_id)
    {
      g_source_remove (priv->timeout_id);
      priv->timeout_id = 0;
    }

  priv->device = NULL;
  priv->sequence = NULL;
  priv->cancelled = FALSE;
  priv->triggered = FALSE;
}

static SugarEventControllerState
sugar_long_press_controller_get_state (SugarEventController *controller)
{
  SugarLongPressController *long_press = SUGAR_LONG_PRESS_CONTROLLER (controller);
  SugarLongPressControllerPrivate *priv = long_press->priv;

  if (!priv->device)
    return SUGAR_EVENT_CONTROLLER_STATE_NONE;

  if (priv->timeout_id)
    return SUGAR_EVENT_CONTROLLER_STATE_COLLECTING;
  else if (priv->cancelled)
    return SUGAR_EVENT_CONTROLLER_STATE_NOT_RECOGNIZED;
  else if (priv->triggered)
    return SUGAR_EVENT_CONTROLLER_STATE_RECOGNIZED;

  return SUGAR_EVENT_CONTROLLER_STATE_NONE;
}

static void
sugar_long_press_controller_reset_impl (SugarEventController *controller)
{
  SugarLongPressController *long_press = SUGAR_LONG_PRESS_CONTROLLER (controller);
  sugar_long_press_controller_reset (long_press);
}

static gboolean
sugar_long_press_controller_handle_event (SugarEventController *controller,
                                          GdkEvent             *event)
{
  SugarLongPressController *long_press = SUGAR_LONG_PRESS_CONTROLLER (controller);
  SugarLongPressControllerPrivate *priv = long_press->priv;
  GdkEventType event_type;
  GdkDevice *device;
  GdkEventSequence *sequence;
  gdouble x, y, root_x, root_y;
  
  event_type = gdk_event_get_event_type (event);
  device = gdk_event_get_device (event);
  
  /* Get event sequence for touch events */
  sequence = gdk_event_get_event_sequence (event);
  
  /* Get coordinates */
  gdk_event_get_position (event, &x, &y);
  
  /* For root coordinates, we need to translate from window coordinates */
  root_x = x;
  root_y = y;

  if (event_type == GDK_BUTTON_PRESS || event_type == GDK_TOUCH_BEGIN)
    {
      /* Only handle primary button for mouse events */
      if (event_type == GDK_BUTTON_PRESS)
        {
          guint button = gdk_button_event_get_button (event);
          if (button != GDK_BUTTON_PRIMARY)
            return FALSE;
        }

      if (priv->device && priv->device != device)
        return FALSE;
      if (priv->sequence && priv->sequence != sequence)
        return FALSE;

      priv->device = device;
      priv->sequence = sequence;
      priv->x = x;
      priv->y = y;
      priv->root_x = root_x;
      priv->root_y = root_y;
      priv->start_time = g_get_monotonic_time ();
      priv->cancelled = FALSE;
      priv->triggered = FALSE;

      priv->timeout_id = g_timeout_add (priv->delay,
                                        sugar_long_press_controller_timeout,
                                        long_press);
      return TRUE;
    }
  else if ((event_type == GDK_BUTTON_RELEASE || event_type == GDK_TOUCH_END) &&
           priv->device == device && priv->sequence == sequence)
    {
      gboolean triggered = priv->triggered;
      
      sugar_long_press_controller_reset (long_press);
      return triggered;
    }
  else if ((event_type == GDK_MOTION_NOTIFY || event_type == GDK_TOUCH_UPDATE) &&
           priv->device == device && priv->sequence == sequence)
    {
      gint dx, dy;

      dx = ABS (priv->root_x - root_x);
      dy = ABS (priv->root_y - root_y);

      if (dx > priv->threshold || dy > priv->threshold)
        {
          priv->cancelled = TRUE;
          sugar_long_press_controller_reset (long_press);
        }

      return FALSE;
    }

  return FALSE;
}

static void
sugar_long_press_controller_class_init (SugarLongPressControllerClass *klass)
{
  GObjectClass *object_class = G_OBJECT_CLASS (klass);
  SugarEventControllerClass *controller_class = SUGAR_EVENT_CONTROLLER_CLASS (klass);

  object_class->finalize = sugar_long_press_controller_finalize;
  object_class->set_property = sugar_long_press_controller_set_property;
  object_class->get_property = sugar_long_press_controller_get_property;

  controller_class->handle_event = sugar_long_press_controller_handle_event;
  controller_class->get_state = sugar_long_press_controller_get_state;
  controller_class->reset = sugar_long_press_controller_reset_impl;

  /**
   * SugarLongPressController::pressed:
   * @controller: the object which received the signal
   * @x: the X coordinate of the press
   * @y: the Y coordinate of the press
   *
   * This signal is emitted whenever a long press gesture happens.
   **/
  signals[PRESSED] =
    g_signal_new ("pressed",
                  G_OBJECT_CLASS_TYPE (object_class),
                  G_SIGNAL_RUN_LAST,
                  G_STRUCT_OFFSET (SugarLongPressControllerClass, pressed),
                  NULL, NULL,
                  NULL,
                  G_TYPE_NONE, 2, G_TYPE_INT, G_TYPE_INT);

  /**
   * SugarLongPressController:delay-factor:
   *
   * Factor by which to change the default long press time of 500ms.
   **/
  g_object_class_install_property (object_class,
                                    PROP_DELAY_FACTOR,
                                    g_param_spec_double ("delay-factor",
                                                         "Delay factor",
                                                         "Factor by which to change the default long press time",
                                                         0.5, 4.0, 1.0,
                                                         G_PARAM_READWRITE |
                                                         G_PARAM_CONSTRUCT_ONLY |
                                                         G_PARAM_STATIC_STRINGS));
}

static void
sugar_long_press_controller_init (SugarLongPressController *controller)
{
  SugarLongPressControllerPrivate *priv;

  controller->priv = sugar_long_press_controller_get_instance_private (controller);
  priv = controller->priv;

  priv->delay = DEFAULT_LONG_PRESS_TIME;
  priv->threshold = TOUCH_THRESHOLD;
}

/**
 * sugar_long_press_controller_new:
 *
 * Creates a new #SugarLongPressController.
 *
 * Returns: (transfer full): a new #SugarLongPressController.
 **/
SugarEventController *
sugar_long_press_controller_new (void)
{
  return g_object_new (SUGAR_TYPE_LONG_PRESS_CONTROLLER,
                       NULL);
}
