#include <gtk/gtk.h>
#include <stdio.h>
#include "ui.h"
#include "drawing.h"
#include "sdl_treatment.h"

void on_export(GtkMenuItem *item, gpointer data)
{
    (void) item;
    (void) data;
    export_current_frame("export.bmp");
}

int main_ui(int x, int y)
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
    GtkDrawingArea* drawing_area = GTK_DRAWING_AREA(
            gtk_builder_get_object(builder, "draw_area"));
    GtkMenuItem *export_button = GTK_MENU_ITEM(
            gtk_builder_get_object(builder, "export-button"));
    // Connects signal handlers
    //      Closing signal
    g_signal_connect(window, "destroy", G_CALLBACK(gtk_main_quit), NULL);
    
    // MenuBar Signal
    g_signal_connect(export_button, "activate", G_CALLBACK(on_export), NULL);
    //gtk_menu_item_activate(export_button);
    //      Drawing signals
    setup_drawing(drawing_area);

    main_sdl(x, y);
    // Runs the main loop
    gtk_main();

    return 0;
}
