#include <gtk/gtk.h>
#include <stdio.h>
#include "ui.h"

cairo_surface_t *surface = NULL;


static void clear_surface (void)
{
  cairo_t *cr;

  cr = cairo_create (surface);

  cairo_set_source_rgb (cr, 1, 1, 1);
  cairo_paint (cr);

  cairo_destroy (cr);
}

void draw_brush (GtkWidget *widget, gdouble x, gdouble y) 
{
    
}


int main_ui()
{
    // Initilizes GTK
    gtk_init(NULL, NULL);

    // Loads the UI description and builds the UI.
    // (Exits if an error occurs.)
    GtkBuilder *builder = gtk_builder_new();
    GError* error = NULL;
    if (gtk_builder_add_from_file(builder, "picsel_glade.glade", &error) == 0)
    {
        g_printerr("Error while loading file: %s\n", error->message);
        g_clear_error(&error);
        return 1;
    }

    // Gets the widgets.
    GtkWindow* window = GTK_WINDOW(gtk_builder_get_object(builder,
                                    "window"));
    GtkDrawingArea* area = GTK_DRAWING_AREA(gtk_builder_get_object(builder,
                                    "draw_area"));
    
    // Connects signal handlers
    


    // Runs the main loop
    gtk_main();
    
    return 0;
}
