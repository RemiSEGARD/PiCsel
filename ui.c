#include <gtk/gtk.h>
#include <stdio.h>
#include "ui.h"
#include "drawing.h"
#include "sdl_treatment.h"

void on_quit()
{
	//free_all call
	gtk_main_quit();
}

void on_prev_frame()
{
	//TODO
}

void on_next_frame()
{
	//TODO
}

void on_new_frame()
{
	//TODO
}

void on_prev_layer()
{
	//TODO
}

void on_next_layer()
{
	//TODO
}

void on_new_layer()
{
	//TODO
}

void on_export(GtkMenuItem *item, gpointer data)
{
    (void) item;
    (void) data;
    export_current_frame("export.bmp");
}

int main_ui(int x, int y, char *filename)
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

    GtkButton* prev_frame_button = GTK_BUTTON(gtk_builder_get_object(builder, "prev_frame"));
    GtkButton* next_frame_button = GTK_BUTTON(gtk_builder_get_object(builder, "next_frame"));
    GtkButton* new_frame_button = GTK_BUTTON(gtk_builder_get_object(builder, "new_frame"));
    GtkButton* prev_layer_button = GTK_BUTTON(gtk_builder_get_object(builder, "prev_layer"));
    GtkButton* next_layer_button = GTK_BUTTON(gtk_builder_get_object(builder, "next_layer"));
    GtkButton* new_layer_button = GTK_BUTTON(gtk_builder_get_object(builder, "new_layer"));


    // Connects signal handlers
    //      Closing signal
    g_signal_connect(window, "destroy", G_CALLBACK(gtk_main_quit), NULL);
    
    // MenuBar Signal
    g_signal_connect(export_button, "activate", G_CALLBACK(on_export), NULL);
    //gtk_menu_item_activate(export_button);
    
    g_signal_connect(prev_frame_button, "clicked", G_CALLBACK(on_prev_frame), NULL);
    g_signal_connect(next_frame_button, "clicked", G_CALLBACK(on_next_frame), NULL);
    g_signal_connect(new_frame_button, "clicked", G_CALLBACK(on_new_frame), NULL);
    g_signal_connect(prev_layer_button, "clicked", G_CALLBACK(on_prev_layer), NULL);
    g_signal_connect(next_layer_button, "clicked", G_CALLBACK(on_next_layer), NULL);
    g_signal_connect(new_layer_button, "clicked", G_CALLBACK(on_new_layer), NULL);
    
    //      Drawing signals
    setup_drawing(drawing_area);
    
    if (x != 0)
        main_sdl(x, y);
    else
    {
        // need to create a thread i think
        // it looks like gtk is not ready yet to receive drawing stuff
        // since gtk_main isnt started
        // BUT we also cant do anything after gtk_main()....
        SDL_Surface *import = main_sdl_import(filename);
        redraw_surface(drawing_area, import);
    }
    
    // Runs the main loop
    gtk_main();
    

    return 0;
}
