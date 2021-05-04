#include <gtk/gtk.h>
#include <stdio.h>
#include "ui.h"
#include "SDL.h"
#include "sdl_treatment.h"

/*typedef enum State
{
    DRAW;
    LINE;
    RECTANGLE;
    CIRCLE;
    FILL;
} State;
*/
/* Surface to store current scribbles */
static cairo_surface_t *surface = NULL;

gdouble x1,y1;

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
static void draw_brush (GtkWidget *widget, gdouble x, gdouble y, GdkRGBA* color)
{
    (void) widget;
    cairo_t *cr;
    
    /* Paint to the surface, where we store our state */
    cr = cairo_create (surface);
    cairo_set_source_rgba(cr, color->red, color->green, color->blue, color->alpha);
    GdkRectangle rect = calculate_coord(x, y, 
            gtk_widget_get_allocated_width(widget),
            gtk_widget_get_allocated_height(widget), color);
    
    //fill(x,y,gtk_widget_get_allocated_width(widget),gtk_widget_get_allocated_height(widget),color);
    
    cairo_rectangle (cr, rect.x, rect.y, rect.width, rect.height);
    cairo_fill (cr);
    
    cairo_destroy (cr);
    
    /* Now invalidate the affected region of the drawing area.
     * Invalidated regions of a widget are redrawn by Gtk
     * In the current case, it redraws the area of the new rectangle
    */
    gtk_widget_queue_draw_area (widget, rect.x, rect.y, 
            rect.width, rect.height);
}


/* Draw a rectangle on the surface at the given position of an image */
static void draw_pixel (GtkWidget *widget, int x, int y, SDL_Surface *s)
{
    (void) widget;
    cairo_t *cr;
    
    /* Paint to the surface, where we store our state */
    cr = cairo_create (surface);
    
    GdkRectangle rect; 
    int w = gtk_widget_get_allocated_width(widget);
    int h = gtk_widget_get_allocated_height(widget);
    int pixel_size;
    if (w < h)
        pixel_size = w / s->w;
    else
        pixel_size = h / s->h;
    rect.x = x * pixel_size;
    rect.y = y * pixel_size;
    rect.width = pixel_size;
    rect.height = pixel_size;
    

    cairo_rectangle (cr, rect.x, rect.y, rect.width, rect.height);
    
    Uint8 r, g, b;
    SDL_GetRGB(get_pixel(s, x, y), s->format, &r, &g, &b);
    cairo_set_source_rgb(cr,
            (double) r / 255, (double) g / 255, (double) b / 255);
    cairo_fill (cr);
    cairo_destroy (cr);
    cairo_set_source_rgb(cr, 0, 0, 0);
    
    /* Now invalidate the affected region of the drawing area.
     * Invalidated regions of a widget are redrawn by Gtk
     * In the current case, it redraws the area of the new rectangle
    */
    gtk_widget_queue_draw_area (widget, rect.x, rect.y, 
            rect.width, rect.height);
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
        x1 = event->x;
        y1 = event->y;
        GdkRGBA* color = malloc(sizeof(GdkRGBA));
        gtk_color_chooser_get_rgba(data,color);
        draw_brush (widget, event->x, event->y, color);
        free(color);
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

    GdkRGBA* color = malloc(sizeof(GdkRGBA));
    gtk_color_chooser_get_rgba(data,color);
    if (event->state & GDK_BUTTON1_MASK)
        draw_brush (widget, event->x, event->y,color);
    free(color);
    return TRUE;
}

void redraw_surface(GtkDrawingArea *drawing_area, SDL_Surface *surface)
{
    for (int i = 0; i < surface->w; i++)
    {
        for (int j = 0; j < surface->h; j++)
        {
            draw_pixel((GtkWidget *)drawing_area, i , j, surface);
        }
    }
}


static gboolean button_release_event_cb (GtkWidget *widget,
        GdkEventMotion *event, gpointer data)
{
    int h = gtk_widget_get_allocated_height(widget);
    int w = gtk_widget_get_allocated_width(widget);
    GdkRGBA* color = malloc(sizeof(GdkRGBA));
    gtk_color_chooser_get_rgba(data,color);
    
    rectangle(x1, y1, event->x, event->y, w, h, color);
    //line(x1,y1,event->x,event->y,w,h,color);
    //circle(x1,y1,event->x,event->y,w,h,color)
    free(color);
    return TRUE;
}
// Setups the events for the drawing area
void setup_drawing(GtkDrawingArea *drawing_area, GtkColorChooser *color_select)
{
    //      Signals for drawing
    //      Drawing areas do not handle clicks, this add the events
    gtk_widget_add_events((GtkWidget *) drawing_area, GDK_BUTTON_PRESS_MASK
            | GDK_POINTER_MOTION_MASK | GDK_BUTTON_RELEASE_MASK);
    //      Handle the backing surface
    g_signal_connect (drawing_area, "draw",
            G_CALLBACK (draw_cb), NULL);
    g_signal_connect (drawing_area,"configure-event",
            G_CALLBACK (configure_event_cb), NULL);
    //      Clicking events
    g_signal_connect (drawing_area, "motion-notify-event",
           G_CALLBACK (motion_notify_event_cb), color_select);
    g_signal_connect (drawing_area, "button-press-event",
            G_CALLBACK (button_press_event_cb), color_select);
    g_signal_connect (drawing_area, "button-release-event",
            G_CALLBACK (button_release_event_cb), color_select);
    return;
}
