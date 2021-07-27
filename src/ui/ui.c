#include <gtk/gtk.h>
#include <stdio.h>
#include "ui.h"
#include "drawing.h"
#include "../sdl/sdl_treatment.h"
#include <string.h>
#include "../sdl/img_frame.h"
#include "../sdl/img_layer.h"
#include "navigation.h"
#include <glib.h>

GtkDrawingArea *darea;

// signal for selecting tools 
gint tpen = 1;
gint teraser = 2;
gint tfill = 3;
gint tline = 4;
gint trectangle = 5;
gint tcircle = 6;

void on_quit()
{
    //free_all call = free liste de layers + les sdl surface
    gtk_main_quit();
}




int w;
int h;

void select_pen(GtkWidget *widget, gpointer data)
{
    gtk_widget_set_sensitive(data, TRUE);
    (void) widget;
    set_pen();
    SDL_Surface *surface = compress_frame(-1, 1);
    redraw_surface(darea, surface);
}

void select_select(GtkWidget *widget, gpointer data)
{
    gtk_widget_set_sensitive(data, FALSE);
    (void) widget;
    set_select();
    SDL_Surface *surface = compress_frame(-1, 1);
    redraw_surface(darea, surface);
}


void select_eraser(GtkWidget *widget, gpointer data)
{
    gtk_widget_set_sensitive(data, TRUE);
    (void) widget;
    set_eraser();
    SDL_Surface *surface = compress_frame(-1, 1);
    redraw_surface(darea, surface);
}

void select_fill(GtkWidget *widget, gpointer data)
{
    gtk_widget_set_sensitive(data, TRUE);
    (void) widget;
    set_fill();
    SDL_Surface *surface = compress_frame(-1, 1);
    redraw_surface(darea, surface);
}

void select_line(GtkWidget *widget, gpointer data)
{
    gtk_widget_set_sensitive(data, TRUE);
    (void) widget;
    set_line();
    SDL_Surface *surface = compress_frame(-1, 1);
    redraw_surface(darea, surface);
}

void select_rectangle(GtkWidget *widget, gpointer data)
{
    gtk_widget_set_sensitive(data, TRUE);
    (void) widget;
    set_rectangle();
    SDL_Surface *surface = compress_frame(-1, 1);
    redraw_surface(darea, surface);
}

void select_circle(GtkWidget *widget, gpointer data)
{
    gtk_widget_set_sensitive(data, TRUE);
    (void) widget;
    set_circle();
    SDL_Surface *surface = compress_frame(-1, 1);
    redraw_surface(darea, surface);
}

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

    //dialog = gtk_file_chooser_dialog_new("Choose a filename", GTK_WINDOW(window), GTK_FILE_CHOOSER_ACTION_SAVE, GTK_STOCK_OK, GTK_RESPONSE_OK, GTK_STOCK_CANCEL, GTK_RESPONSE_CANCEL, NULL);
	
    dialog = gtk_file_chooser_dialog_new("Choose a filename", GTK_WINDOW(window), GTK_FILE_CHOOSER_ACTION_SAVE, "_OK" ,GTK_RESPONSE_OK, "_Cancel", GTK_RESPONSE_CANCEL, NULL);
    gtk_window_set_transient_for(window, (GtkWindow *)dialog);
	gtk_widget_show(dialog);
	gint resp = gtk_dialog_run(GTK_DIALOG(dialog));
	if(resp != GTK_RESPONSE_NONE)
		res = gtk_file_chooser_get_filename(GTK_FILE_CHOOSER(dialog));
	gtk_widget_destroy(dialog);
	return (char*)res;
}


void on_import(GtkWidget *widget, gpointer window)
{
    (void)widget;
    draw_background();
	char* filename = open_dialog(window);
    if(filename)
    {
        main_sdl(-1, -1, filename);
		SDL_Surface *surface = compress_frame(-1, 1);
		redraw_surface(darea, surface);
        add_grid_buttons();
	}
}


void on_export_sprite(GtkMenuItem *item, gpointer window, gpointer data)
{
    (void) item;
    (void) data;
    char* expname = open_dialog(window);
    if(expname)
    	export_sprite(expname);
}

void on_export_gif(GtkMenuItem *item, gpointer window, gpointer data)
{
    (void) item;
    (void) data;
    char* expname = open_dialog(window);
    if(expname)
    	export_current_gif(expname);
}
void on_export_picsel(GtkMenuItem *item, gpointer window, gpointer data)
{
    (void) item;
    (void) data;
    char* expname = open_dialog(window);
    if(expname)
    	export_current_frame(expname);
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


    //GtkButton* prev_frame_button = GTK_BUTTON(gtk_builder_get_object(builder, "prev_frame"));
    //GtkButton* next_frame_button = GTK_BUTTON(gtk_builder_get_object(builder, "next_frame"));
    //GtkButton* new_frame_button = GTK_BUTTON(gtk_builder_get_object(builder, "new_frame"));
    //GtkButton* prev_layer_button = GTK_BUTTON(gtk_builder_get_object(builder, "prev_layer"));
    //GtkButton* next_layer_button = GTK_BUTTON(gtk_builder_get_object(builder, "next_layer"));
    //GtkButton* new_layer_button = GTK_BUTTON(gtk_builder_get_object(builder, "new_layer"));



    // button for slecting tools

    GtkButton* pen_button = GTK_BUTTON(gtk_builder_get_object(builder, "pen"));
    GtkButton* eraser_button = GTK_BUTTON(gtk_builder_get_object(builder, "eraser"));
    GtkButton* fill_button = GTK_BUTTON(gtk_builder_get_object(builder, "fill"));
    GtkButton* line_button = GTK_BUTTON(gtk_builder_get_object(builder, "line"));
    GtkButton* rectangle_button = GTK_BUTTON(gtk_builder_get_object(builder, "rectangle"));
    GtkButton* circle_button = GTK_BUTTON(gtk_builder_get_object(builder, "circle"));
    GtkButton* select_button = GTK_BUTTON(gtk_builder_get_object(builder, "select"));

    GtkMenuItem* export_button_img = GTK_MENU_ITEM(gtk_builder_get_object(builder, "export-button-img"));
    GtkMenuItem* export_picsel_button = GTK_MENU_ITEM(gtk_builder_get_object(builder, "save-button"));
    GtkMenuItem* open_item = GTK_MENU_ITEM(gtk_builder_get_object(builder, "open-item"));
    GtkMenuItem* export_button_sprite = GTK_MENU_ITEM(gtk_builder_get_object(builder, "export-button-ss"));
    GtkMenuItem* export_button_gif = GTK_MENU_ITEM(gtk_builder_get_object(builder, "export-button-gif"));

    main_sdl(x, y, filename);


    // Connects signal handlers
    //      Closing signal
    g_signal_connect(window, "destroy", G_CALLBACK(gtk_main_quit), NULL);
    GtkMenuItem* quit_button = GTK_MENU_ITEM(gtk_builder_get_object(builder, "quit_button"));
    g_signal_connect(quit_button, "activate", G_CALLBACK(gtk_main_quit), NULL);
    

    // tools signal

    g_signal_connect(pen_button, "clicked", G_CALLBACK(select_pen), color_select);
    g_signal_connect(select_button, "clicked", G_CALLBACK(select_select), color_select);
    g_signal_connect(eraser_button, "clicked", G_CALLBACK(select_eraser), color_select);
    g_signal_connect(fill_button, "clicked", G_CALLBACK(select_fill), color_select);
    g_signal_connect(line_button, "clicked", G_CALLBACK(select_line), color_select);
    g_signal_connect(rectangle_button, "clicked", G_CALLBACK(select_rectangle), color_select);
    g_signal_connect(circle_button, "clicked", G_CALLBACK(select_circle), color_select);

    //      Drawing signals
    setup_drawing(drawing_area, color_select, window);

    w = gtk_widget_get_allocated_width((GtkWidget *)drawing_area);
    h = gtk_widget_get_allocated_height((GtkWidget *)drawing_area);

    g_signal_connect(drawing_area, "draw", G_CALLBACK(on_drawingarea_draw), NULL);

    //connects the menu items
    g_signal_connect(export_button_img, "activate", G_CALLBACK(on_export), window);
    g_signal_connect(export_picsel_button, "activate", G_CALLBACK(on_export_picsel), window);
    g_signal_connect(export_button_sprite, "activate", G_CALLBACK(on_export_sprite), window);
    g_signal_connect(export_button_gif, "activate", G_CALLBACK(on_export_gif), window);
    g_signal_connect(open_item, "activate", G_CALLBACK(on_import), window);


    setup_nav(builder);
    
    // Runs the main loop
    gtk_main();

    return 0;
}
