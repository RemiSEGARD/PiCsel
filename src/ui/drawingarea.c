#include <gtk/gtk.h>
#include "inputs.h"

#include "../sdl/sdl_treatment.h"

void draw_background()
{
    cairo_t *cr;

    cr = cairo_create (surface);

    cairo_set_source_rgb (cr, 0.94901, 0.94509, 0.94117);
    cairo_paint (cr);

    cairo_destroy (cr);

}

int win_x;
int win_y;

void on_drawingarea_draw(GtkWidget *widget, gpointer data)
{
    (void) data;
    int new_x = gtk_widget_get_allocated_width(widget);
    int new_y = gtk_widget_get_allocated_height(widget);
    if (win_x != new_x || win_y != new_y)
    {
        win_x = new_x;
        win_y = new_y;
        SDL_Surface *surface = compress_frame(-1, 1);
        redraw_surface((GtkDrawingArea *)widget, surface);
    }
}

/* Create a new surface of the appropriate size to store our scribbles */
gboolean configure_event_cb (GtkWidget *widget, 
        GdkEventConfigure *event, gpointer data)
{
    (void) event;
    (void) data;
    if (surface)
        cairo_surface_destroy (surface);

    surface = gdk_window_create_similar_surface (gtk_widget_get_window (widget),
            CAIRO_CONTENT_COLOR, 
            gtk_widget_get_allocated_width(widget), 
            gtk_widget_get_allocated_height(widget));
    
    /* Initialize the surface to white */
    draw_background();

    return TRUE;
}

/* Redraw the screen from the surface. Note that the ::draw
 * signal receives a ready-to-be-used cairo_t that is already
 * clipped to only draw the exposed areas of the widget
 */
gboolean draw_cb(GtkWidget *widget, cairo_t *cr, gpointer data)
{
    (void) widget;
    (void) data;
    cairo_set_source_surface (cr, surface, 0, 0);
    cairo_paint (cr);

    return FALSE;
}

/* Draw a rectangle on the surface at the given position */
void draw_brush (GtkWidget *widget, gdouble x, gdouble y, GdkRGBA* color)
{
    (void) widget;
    cairo_t *cr;
    
    /* Paint to the surface, where we store our state */
    cr = cairo_create (surface);
    GdkRectangle rect = calculate_coord(x, y, win_x,
            win_y, color);
    
    cairo_set_source_rgba(cr, color->red, color->green, color->blue, color->alpha);
    
    //fill(x,y,win_x,win_y,color);
    
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
void draw_pixel (GtkWidget *widget, int x, int y, SDL_Surface *s)
{
    (void) widget;
    cairo_t *cr;
    
    /* Paint to the surface, where we store our state */
    cr = cairo_create (surface);
    
    GdkRectangle rect; 
    int w = win_x;
    int h = win_y;
    int pixel_size;
    if ((double)(s->w) / w > (double)(s->h) / h)
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

void redraw_surface(GtkDrawingArea *drawing_area, SDL_Surface *surf)
{
    cairo_t *cr;
    
    /* Paint to the surface, where we store our state */
    cr = cairo_create (surface);
    
    GdkRectangle rect; 
    int w = win_x;
    int h = win_y;
    int pixel_size;
    if ((double)(surf->w) / w > (double)(surf->h) / h)
        pixel_size = w / surf->w;
    else
        pixel_size = h / surf->h;
    rect.x = surf->w * pixel_size;
    rect.y = 0;
    rect.width = 3;
    rect.height = surf->h * pixel_size + 3;
    
    cairo_rectangle (cr, rect.x, rect.y, rect.width, rect.height);
    
    cairo_set_source_rgb(cr, 0, 0, 0);
    cairo_fill (cr);
    cairo_destroy (cr);
    gtk_widget_queue_draw_area ((GtkWidget *)drawing_area, rect.x, rect.y, 
            rect.width, rect.height);
    
    /* Paint to the surface, where we store our state */
    cr = cairo_create (surface);
    
    rect.x = 0;
    rect.y = surf->h * pixel_size;
    rect.width = surf->w * pixel_size;
    rect.height = 3;
    
    cairo_rectangle (cr, rect.x, rect.y, rect.width, rect.height);
    
    cairo_set_source_rgb(cr, 0, 0, 0);
    cairo_fill (cr);
    cairo_destroy (cr);
    
    gtk_widget_queue_draw_area((GtkWidget *)drawing_area, rect.x, rect.y, 
            rect.width, rect.height);
    for (int i = 0; i < surf->w; i++)
    {
        for (int j = 0; j < surf->h; j++)
        {
            draw_pixel((GtkWidget *)drawing_area, i , j, surf);
        }
    }
}
