/* GTK - The GIMP Toolkit
 * Copyright (C) 1995-1997 Peter Mattis, Spencer Kimball and Josh MacDonald
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Library General Public
 * License as published by the Free Software Foundation; either
 * version 2 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Library General Public License for more details.
 *
 * You should have received a copy of the GNU Library General Public
 * License along with this library; if not, write to the
 * Free Software Foundation, Inc., 59 Temple Place - Suite 330,
 * Boston, MA 02111-1307, USA.
 */

#include <gtk/gtk.h>

/* Backing pixmap for drawing area */
static GdkPixmap *pixmap = NULL;

/* Create a new backing pixmap of the appropriate size */
static gboolean
configure_event (GtkWidget *widget, GdkEventConfigure *event)
{
  if (pixmap)
     g_object_unref (pixmap);

  pixmap = gdk_pixmap_new (widget->window,
                           widget->allocation.width,
                           widget->allocation.height,
                           -1);
  gdk_draw_rectangle (pixmap,
                      widget->style->white_gc,
                      TRUE,
                      0, 0,
                      widget->allocation.width,
                      widget->allocation.height);

  return TRUE;
}

/* Redraw the screen from the backing pixmap */
static gboolean
expose_event (GtkWidget *widget, GdkEventExpose *event)
{
  gdk_draw_drawable (widget->window,
                     widget->style->fg_gc[gtk_widget_get_state (widget)],
                     pixmap,
                     event->area.x, event->area.y,
                     event->area.x, event->area.y,
                     event->area.width, event->area.height);

  return FALSE;
}

/* Draw a rectangle on the screen, size depending on pressure,
   and color on the type of device */
static void
draw_brush (GtkWidget *widget, GdkInputSource source,
            gdouble x, gdouble y, gdouble pressure)
{
  GdkGC *gc;
  GdkRectangle update_rect;

  switch (source)
    {
    case GDK_SOURCE_MOUSE:
      gc = widget->style->dark_gc[gtk_widget_get_state (widget)];
      break;
    case GDK_SOURCE_PEN:
      gc = widget->style->black_gc;
      break;
    case GDK_SOURCE_ERASER:
      gc = widget->style->white_gc;
      break;
    default:
      gc = widget->style->light_gc[gtk_widget_get_state (widget)];
    }

  update_rect.x = x - 10 * pressure;
  update_rect.y = y - 10 * pressure;
  update_rect.width = 20 * pressure;
  update_rect.height = 20 * pressure;
  gdk_draw_rectangle (pixmap, gc, TRUE,
                      update_rect.x, update_rect.y,
                      update_rect.width, update_rect.height);
  gtk_widget_queue_draw_area (widget, 
                      update_rect.x, update_rect.y,
                      update_rect.width, update_rect.height);
}

static void
print_button_press (GdkDevice *device)
{
  g_print ("Button press on device '%s'\n", device->name);
}

static gboolean
button_press_event (GtkWidget *widget, GdkEventButton *event)
{
  print_button_press (event->device);
  
  if (event->button == 1 && pixmap != NULL) {
    gdouble pressure;
    gdk_event_get_axis ((GdkEvent *)event, GDK_AXIS_PRESSURE, &pressure);
    draw_brush (widget, event->device->source, event->x, event->y, pressure);
  }

  return TRUE;
}

static gboolean
motion_notify_event (GtkWidget *widget, GdkEventMotion *event)
{
  gdouble x, y;
  gdouble pressure;
  GdkModifierType state;

  if (event->is_hint) 
    {
      gdk_device_get_state (event->device, event->window, NULL, &state);
      gdk_event_get_axis ((GdkEvent *)event, GDK_AXIS_X, &x);
      gdk_event_get_axis ((GdkEvent *)event, GDK_AXIS_Y, &y);
      gdk_event_get_axis ((GdkEvent *)event, GDK_AXIS_PRESSURE, &pressure);
    }
  else
    {
      x = event->x;
      y = event->y;
      gdk_event_get_axis ((GdkEvent *)event, GDK_AXIS_PRESSURE, &pressure);
      state = event->state;
    }
    
  if (state & GDK_BUTTON1_MASK && pixmap != NULL)
    draw_brush (widget, event->device->source, x, y, pressure);
  
  return TRUE;
}

void
input_dialog_destroy (GtkWidget *w, gpointer data)
{
  *((GtkWidget **)data) = NULL;
}

void
create_input_dialog ()
{
  static GtkWidget *inputd = NULL;

  if (!inputd)
    {
      inputd = gtk_input_dialog_new();

      g_signal_connect (inputd, "destroy",
                        G_CALLBACK (input_dialog_destroy), (gpointer) &inputd);
      g_signal_connect_swapped (GTK_INPUT_DIALOG (inputd)->close_button,
                                "clicked",
                                G_CALLBACK (gtk_widget_hide),
                                inputd);
      gtk_widget_hide (GTK_INPUT_DIALOG (inputd)->save_button);

      gtk_widget_show (inputd);
    }
  else
    {
      if (!gtk_widget_get_mapped (inputd))
        gtk_widget_show (inputd);
      else
        gdk_window_raise (inputd->window);
    }
}

