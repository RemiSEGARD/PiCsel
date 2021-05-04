#include <gtk/gtk.h>
#include <stdio.h>
#include "ui.h"
#include "drawing.h"
#include "sdl_treatment.h"
#include <string.h>

GtkDrawingArea *darea;

void on_quit()
{
    //free_all call = free liste de layers + les sdl surface
    gtk_main_quit();
}

void on_prev_frame()
{
    prev_frame();
    SDL_Surface *surface = compress_frame(-1, 1);
    redraw_surface(darea, surface);
}

void on_next_frame()
{
    next_frame();
    SDL_Surface *surface = compress_frame(-1, 1);
    redraw_surface(darea, surface);

}

void on_new_frame()
{
    new_frame();
}

void on_prev_layer()
{
    prev_layer();
}

void on_next_layer()
{
    next_layer();
}

void on_new_layer()
{
    new_layer();
}

int w;
int h;

void on_drawingarea_draw(GtkWidget *widget, gpointer data)
{
    (void) data;
    int new_w = gtk_widget_get_allocated_width(widget);
    int new_h = gtk_widget_get_allocated_height(widget);
    if (w != new_w || h != new_h)
    {
        w = gtk_widget_get_allocated_width(widget);
        h = gtk_widget_get_allocated_height(widget);
        SDL_Surface *surface = compress_frame(-1, 1);
        redraw_surface((GtkDrawingArea *)widget, surface);
    }
}

char* open_dialog(gpointer window)
//static void open_dialog(GtkWidget* button, gpointer window)
{
	gchar* res = NULL;
	GtkWidget *dialog;
	dialog = gtk_file_chooser_dialog_new("Choose a filename", GTK_WINDOW(window), GTK_FILE_CHOOSER_ACTION_SAVE, GTK_STOCK_OK, GTK_RESPONSE_OK, GTK_STOCK_CANCEL, GTK_RESPONSE_CANCEL, NULL);
	gtk_widget_show_all(dialog);
	gint resp = gtk_dialog_run(GTK_DIALOG(dialog));
	if(resp == GTK_RESPONSE_OK)
		res = gtk_file_chooser_get_filename(GTK_FILE_CHOOSER(dialog));
	gtk_widget_destroy(dialog);
	printf("%s\n", res);
	return (char*)res;
}

void on_export(GtkMenuItem *item, gpointer window, gpointer data)
{
    (void) item;
    (void) data;
    char* expname = open_dialog(window);
    if(expname)
    	export_current_frame(expname);
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
    GtkWindow* window = GTK_WINDOW(gtk_builder_get_object(builder, "window"));
    GtkDrawingArea* drawing_area = GTK_DRAWING_AREA(gtk_builder_get_object(builder, "draw_area"));
    darea = drawing_area;

    GtkColorChooser* color_select = GTK_COLOR_CHOOSER(gtk_builder_get_object(builder, "color"));


    GtkButton* prev_frame_button = GTK_BUTTON(gtk_builder_get_object(builder, "prev_frame"));
    GtkButton* next_frame_button = GTK_BUTTON(gtk_builder_get_object(builder, "next_frame"));
    GtkButton* new_frame_button = GTK_BUTTON(gtk_builder_get_object(builder, "new_frame"));
    GtkButton* prev_layer_button = GTK_BUTTON(gtk_builder_get_object(builder, "prev_layer"));
    GtkButton* next_layer_button = GTK_BUTTON(gtk_builder_get_object(builder, "next_layer"));
    GtkButton* new_layer_button = GTK_BUTTON(gtk_builder_get_object(builder, "new_layer"));

    GtkMenuItem* export_button = GTK_MENU_ITEM(gtk_builder_get_object(builder, "export-button"));
    GtkMenuItem* open_item = GTK_MENU_ITEM(gtk_builder_get_object(builder, "open-item"));

    if (x != 0)
        main_sdl(x, y);
    else
    {
        // need to create a thread i think
        // it looks like gtk is not ready yet to receive drawing stuff
        // since gtk_main isnt started
        // BUT we also cant do anything after gtk_main()....
        main_sdl_import(filename);
    }


    // Connects signal handlers
    //      Closing signal
    g_signal_connect(window, "destroy", G_CALLBACK(gtk_main_quit), NULL);

    // MenuBar Signal
    //gtk_menu_item_activate(export_button);

    g_signal_connect(prev_frame_button, "clicked", G_CALLBACK(on_prev_frame), NULL);
    g_signal_connect(next_frame_button, "clicked", G_CALLBACK(on_next_frame), NULL);
    g_signal_connect(new_frame_button, "clicked", G_CALLBACK(on_new_frame), NULL);
    g_signal_connect(prev_layer_button, "clicked", G_CALLBACK(on_prev_layer), NULL);
    g_signal_connect(next_layer_button, "clicked", G_CALLBACK(on_next_layer), NULL);
    g_signal_connect(new_layer_button, "clicked", G_CALLBACK(on_new_layer), NULL);

    //      Drawing signals
    setup_drawing(drawing_area, color_select);

    w = gtk_widget_get_allocated_width((GtkWidget *)drawing_area);
    h = gtk_widget_get_allocated_height((GtkWidget *)drawing_area);

    g_signal_connect(drawing_area, "draw", G_CALLBACK(on_drawingarea_draw), NULL);

    //connects the menu items
    g_signal_connect(export_button, "activate", G_CALLBACK(on_export), NULL);
    g_signal_connect(open_item, "activate", G_CALLBACK(open_dialog), window);


    //open_dialog(window);
    
    // Runs the main loop
    gtk_main();

    return 0;
}
