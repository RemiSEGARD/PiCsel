#include <gtk/gtk.h>
#include <stdio.h>
#include "ui.h"

/* Surface to store current scribbles */
static cairo_surface_t *surface = NULL;

/* Sets the whole surface to white */
static void clear_surface (void)
{
    cairo_t *cr;

    cr = cairo_create (surface);

    cairo_set_source_rgb (cr, 1, 1, 1);
    cairo_paint (cr);

    cairo_destroy (cr);
}

/* Create a new surface of the appropriate size to store our scribbles */
static gboolean configure_event_cb (GtkWidget *widget, 
        GdkEventConfigure *event, gpointer data)
{
    (void) event;
    (void) data;
    if (surface)
        cairo_surface_destroy (surface);

    surface = gdk_window_create_similar_surface (gtk_widget_get_window (widget),
            CAIRO_CONTENT_COLOR,
            gtk_widget_get_allocated_width (widget),
            gtk_widget_get_allocated_height (widget));
    
    /* Initialize the surface to white */
    clear_surface ();

    return TRUE;
}

/* Redraw the screen from the surface. Note that the ::draw
 * signal receives a ready-to-be-used cairo_t that is already
 * clipped to only draw the exposed areas of the widget
 */
static gboolean draw_cb(GtkWidget *widget, cairo_t *cr, gpointer data)
{
    (void) widget;
    (void) data;
    cairo_set_source_surface (cr, surface, 0, 0);
    cairo_paint (cr);

    return FALSE;
}

/* Draw a rectangle on the surface at the given position */
static void draw_brush (GtkWidget *widget, gdouble x, gdouble y)
{
    (void) widget;
    cairo_t *cr;
    
    /* Paint to the surface, where we store our state */
    cr = cairo_create (surface);

    cairo_rectangle (cr, x - 3, y - 3, 6, 6);
    cairo_fill (cr);

    cairo_destroy (cr);
    
    /* Now invalidate the affected region of the drawing area.
     * Invalidated regions of a widget are redrawn by Gtk
     * In the current case, it redraws the area of the new rectangle
    */
    gtk_widget_queue_draw_area (widget, x - 3, y - 3, 6, 6);
}

/* Handle the different types of button pressed */
static gboolean button_press_event_cb (GtkWidget *widget,
        GdkEventButton *event, gpointer data)
{
    (void) data;
    if (surface == NULL)
        return FALSE;

    if (event->button == GDK_BUTTON_PRIMARY)
    {
        draw_brush (widget, event->x, event->y);
    }
    else if (event->button == GDK_BUTTON_SECONDARY)
    {
        clear_surface ();
        gtk_widget_queue_draw (widget);
    }

    return TRUE;
}

/* Handle the held mouse button 1 event */
static gboolean motion_notify_event_cb (GtkWidget *widget,
        GdkEventMotion *event, gpointer data)
{
    (void) data;
    if (surface == NULL)
        return FALSE;

    if (event->state & GDK_BUTTON1_MASK)
        draw_brush (widget, event->x, event->y);

    return TRUE;
}

// Setups the events for the drawing area
void setup_drawing(GtkDrawingArea *drawing_area)
{
    //      Signals for drawing
    //      Drawing areas do not handle clicks, this add the events
    gtk_widget_add_events((GtkWidget *) drawing_area, GDK_BUTTON_PRESS_MASK
            | GDK_POINTER_MOTION_MASK);
    //      Handle the backing surface
    g_signal_connect (drawing_area, "draw",
            G_CALLBACK (draw_cb), NULL);
    g_signal_connect (drawing_area,"configure-event",
            G_CALLBACK (configure_event_cb), NULL);
    //      Clicking events
    g_signal_connect (drawing_area, "motion-notify-event",
            G_CALLBACK (motion_notify_event_cb), NULL);
    g_signal_connect (drawing_area, "button-press-event",
            G_CALLBACK (button_press_event_cb), NULL);

    return;
}
