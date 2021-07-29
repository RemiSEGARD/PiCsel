#ifndef DRAWINGAREA_H
#define DRAWINGAREA_H

void draw_background();

gboolean configure_event_cb (GtkWidget *widget, 
        GdkEventConfigure *event, gpointer data);

gboolean draw_cb(GtkWidget *widget, cairo_t *cr, gpointer data);

void draw_brush (GtkWidget *widget, gdouble x, gdouble y, GdkRGBA* color);

void draw_pixel (GtkWidget *widget, int x, int y, SDL_Surface *s);

void redraw_surface(GtkDrawingArea *drawing_area, SDL_Surface *surf);

#endif
