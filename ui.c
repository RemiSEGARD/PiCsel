#include <gtk/gtk.h>
#include <stdio.h>
#include "ui.h"
#include "drawing.h"
#include "sdl_treatment.h"
#include <string.h>
#include "img_frame.h"
#include "img_layer.h"
#include <glib.h>

GtkDrawingArea *darea;

struct SDL_data *sdl_dat = NULL;

GtkWidget *curframe_button;
GtkWidget *curlayer_button;
GtkGrid* f_grid;
GtkGrid* l_grid;

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


void choose_frame(GtkWidget* widget, gpointer data)
{
    (void) widget;
    select_layer(*((int *)data), sdl_dat->curlayer);
    SDL_Surface *surface = compress_frame(-1, 1);
    redraw_surface(darea, surface);
    gtk_widget_set_sensitive(curframe_button, TRUE);
    gtk_widget_set_sensitive(widget, FALSE);
    curframe_button = widget;
    
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

void on_new_frame(GtkWidget *widget, gpointer data)
{
    (void) widget;
    char label[10];
    sprintf(label, "%u", sdl_dat->nbframe + 1);
    GtkWidget *button = gtk_button_new_with_label(label);
    
    new_frame();
    gtk_grid_insert_column(data, sdl_dat->nbframe);
    gtk_grid_attach(data, button, sdl_dat->nbframe, 0, 1, 1);
    gtk_widget_show(button);
    int *index = (int *)malloc(sizeof(int));
    *index = sdl_dat->nbframe - 1;
    g_signal_connect(button, "clicked", G_CALLBACK(choose_frame), index);
}

void choose_layer(GtkWidget *widget, gpointer data)
{
    (void) widget;
    select_layer(sdl_dat->curframe, *((int *)data));
    SDL_Surface *surface = compress_frame(-1, 1);
    redraw_surface(darea, surface);
    gtk_widget_set_sensitive(curlayer_button, TRUE);
    gtk_widget_set_sensitive(widget, FALSE);
    curlayer_button = widget;
}

void hide_show_layer(GtkWidget *widget, gpointer data)
{
    (void) widget;
    Frame *f = sdl_dat->frames->next;
    while (f != NULL)
    {
        Layer *l = f->layer->next;
        int n = *((int *)data);
        while (n > 0)
        {
            l = l->next;
            n--;
        }
        if (l->shown)
            l->shown = 0;
        else
            l->shown = 1;
        f = f->next;
    }
    SDL_Surface *surface = compress_frame(-1, 1);
    redraw_surface(darea, surface);
}

void on_prev_layer()
{
    prev_layer();
}

void on_next_layer()
{
    next_layer();
}

void on_new_layer(GtkWidget *widget, gpointer data)
{
    (void) widget;
    char label[10];
    sprintf(label, "%u", sdl_dat->nblayer + 1);
    GtkWidget *button = gtk_button_new_with_label(label);
    
    new_layer();
    gtk_grid_insert_column(data, sdl_dat->nblayer);
    gtk_grid_attach(data, button, sdl_dat->nblayer, 0, 1, 1);
    gtk_widget_show(button);
    int *index = (int *) malloc(sizeof(int));
    *index = sdl_dat->nblayer - 1;
    g_signal_connect(button, "clicked", G_CALLBACK(choose_layer), index);
    
    button = gtk_toggle_button_new_with_label("");
    
    gtk_grid_attach(data, button, sdl_dat->nblayer, 1, 1, 1);
    gtk_widget_show(button);
    g_signal_connect(button, "clicked", G_CALLBACK(hide_show_layer), index);
}

int w;
int h;

void select_pen()
{
    set_pen();
}

void select_eraser()
{
    set_eraser();
}

void select_fill()
{
    set_fill();
}

void select_line()
{
    set_line();
}

void select_rectangle()
{
    set_rectangle();
}

void select_circle()
{
    set_circle();
}

guint id = 0;

gboolean play(gpointer data)
{
    (void) data;
    on_next_frame();
    return TRUE;
}

void play_animation()
{
    if (sdl_dat == NULL)
        sdl_dat = get_sdl_data();
    if (id == 0)
        id = g_timeout_add(get_frame(sdl_dat->curframe)->duration, play, NULL);
    else
    {
        g_source_remove(id);
        id = 0;
    }
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

void add_grid_buttons()
{
    GtkWidget *button = gtk_button_new_with_label("1");
    
    gtk_grid_insert_column(f_grid, 0);
    gtk_grid_attach(f_grid, button, 0, 0, 1, 1);
    gtk_widget_show(button);
    int *index = (int *)malloc(sizeof(int));
    *index = 0;
    g_signal_connect(button, "clicked", G_CALLBACK(choose_frame), index);
    gtk_widget_set_sensitive(button, FALSE);
    curframe_button = button;
    
    button = gtk_button_new_with_label("1");
    
    gtk_grid_insert_column(l_grid, 0);
    gtk_grid_attach(l_grid, button, 0, 0, 1, 1);
    gtk_widget_show(button);
    g_signal_connect(button, "clicked", G_CALLBACK(choose_layer), index);
    gtk_widget_set_sensitive(button, FALSE);
    curlayer_button = button;

    button = gtk_toggle_button_new_with_label("");
    
    gtk_grid_attach(l_grid, button, 0, 1, 1, 1);
    gtk_widget_show(button);
    g_signal_connect(button, "clicked", G_CALLBACK(hide_show_layer), index);


    if (sdl_dat != NULL)
    {
        char label[10];
        GtkWidget *button;
        int *index;
        for (int i = 0; i < sdl_dat->nbframe - 1; i++)
        {
            sprintf(label, "%u", i + 2);
            button = gtk_button_new_with_label(label);

            gtk_grid_insert_column(f_grid, i + 1);
            gtk_grid_attach(f_grid, button, i + 1, 0, 1, 1);
            gtk_widget_show(button);
            index = (int *)malloc(sizeof(int));
            *index = i + 1;
            g_signal_connect(button, "clicked", G_CALLBACK(choose_frame), index);
        }
        for (int i = 0; i < sdl_dat->nblayer - 1; i++)
        {
            sprintf(label, "%u", i + 2);
            button = gtk_button_new_with_label(label);

            gtk_grid_insert_column(l_grid, i + 1);
            gtk_grid_attach(l_grid, button, i + 1, 0, 1, 1);
            gtk_widget_show(button);
            index = (int *) malloc(sizeof(int));
            *index = i + 1;
            g_signal_connect(button, "clicked", G_CALLBACK(choose_layer), index);

            button = gtk_toggle_button_new_with_label("");

            gtk_grid_attach(l_grid, button, i + 1, 1, 1, 1);
            gtk_widget_show(button);
            g_signal_connect(button, "clicked", G_CALLBACK(hide_show_layer), index);
        }
    }
}

void reset_grids()
{
    for (int i = 0; i <= sdl_dat->nbframe; i++)
    {
        gtk_grid_remove_column(f_grid, 0);
    }
    for (int i = 0; i <= sdl_dat->nblayer; i++)
    {
        gtk_grid_remove_column(l_grid, 0);
    }
}

void on_import(GtkWidget *widget, gpointer window)
{
    (void)widget;
	char* filename = open_dialog(window);
    if(filename)
    { 
        reset_grids();
        if (g_str_has_suffix(filename, ".picsel"))
        {
            main_picsel_import(filename);
        }
        else
        {
            main_sdl_import(filename);
        }


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

    GtkButton* add_frame = GTK_BUTTON(gtk_builder_get_object(builder, "add_frame"));
    GtkButton* add_layer = GTK_BUTTON(gtk_builder_get_object(builder, "add_layer"));
    GtkGrid* frame_grid = GTK_GRID(gtk_builder_get_object(builder, "frame_grid"));
    GtkGrid* layer_grid = GTK_GRID(gtk_builder_get_object(builder, "layer_grid"));

    f_grid = frame_grid;
    l_grid = layer_grid;

    // button for slecting tools

    GtkButton* pen_button = GTK_BUTTON(gtk_builder_get_object(builder, "pen"));
    GtkButton* eraser_button = GTK_BUTTON(gtk_builder_get_object(builder, "eraser"));
    GtkButton* fill_button = GTK_BUTTON(gtk_builder_get_object(builder, "fill"));
    GtkButton* line_button = GTK_BUTTON(gtk_builder_get_object(builder, "line"));
    GtkButton* rectangle_button = GTK_BUTTON(gtk_builder_get_object(builder, "rectangle"));
    GtkButton* circle_button = GTK_BUTTON(gtk_builder_get_object(builder, "circle"));

    GtkButton* play_button = GTK_BUTTON(gtk_builder_get_object(builder, "play"));

    GtkMenuItem* export_button_img = GTK_MENU_ITEM(gtk_builder_get_object(builder, "export-button-img"));
    GtkMenuItem* export_picsel_button = GTK_MENU_ITEM(gtk_builder_get_object(builder, "save-button"));
    GtkMenuItem* open_item = GTK_MENU_ITEM(gtk_builder_get_object(builder, "open-item"));
    GtkMenuItem* export_button_sprite = GTK_MENU_ITEM(gtk_builder_get_object(builder, "export-button-ss"));
    GtkMenuItem* export_button_gif = GTK_MENU_ITEM(gtk_builder_get_object(builder, "export-button-gif"));

    if (x != 0)
        main_sdl(x, y);
    else if (g_str_has_suffix(filename, ".picsel"))
    {
        main_picsel_import(filename);
    }
    else if (g_str_has_suffix(filename, ".gif"))
    {
        main_gif_import(filename);
    }
    else
    {
        main_sdl_import(filename);
    }


    // Connects signal handlers
    //      Closing signal
    g_signal_connect(window, "destroy", G_CALLBACK(gtk_main_quit), NULL);

    // MenuBar Signal
    //gtk_menu_item_activate(export_button);

    //g_signal_connect(prev_frame_button, "clicked", G_CALLBACK(on_prev_frame), NULL);
    //g_signal_connect(next_frame_button, "clicked", G_CALLBACK(on_next_frame), NULL);
    //g_signal_connect(new_frame_button, "clicked", G_CALLBACK(on_new_frame), NULL);
    //g_signal_connect(prev_layer_button, "clicked", G_CALLBACK(on_prev_layer), NULL);
    //g_signal_connect(next_layer_button, "clicked", G_CALLBACK(on_next_layer), NULL);
    //g_signal_connect(new_layer_button, "clicked", G_CALLBACK(on_new_layer), NULL);
    
    g_signal_connect(add_frame, "clicked", G_CALLBACK(on_new_frame), frame_grid);
    g_signal_connect(add_layer, "clicked", G_CALLBACK(on_new_layer), layer_grid);
    
    if (sdl_dat == NULL)
        sdl_dat = get_sdl_data();
    
    // button 1 for layers and frame
/*    
    GtkWidget *button = gtk_button_new_with_label("1");
    
    gtk_grid_insert_column(frame_grid, 0);
    gtk_grid_attach(frame_grid, button, 0, 0, 1, 1);
    gtk_widget_show(button);
    int *index = (int *)malloc(sizeof(int));
    *index = 0;
    g_signal_connect(button, "clicked", G_CALLBACK(choose_frame), index);
    gtk_widget_set_sensitive(button, FALSE);
    curframe_button = button;
    
    button = gtk_button_new_with_label("1");
    
    gtk_grid_insert_column(layer_grid, 0);
    gtk_grid_attach(layer_grid, button, 0, 0, 1, 1);
    gtk_widget_show(button);
    g_signal_connect(button, "clicked", G_CALLBACK(choose_layer), index);
    gtk_widget_set_sensitive(button, FALSE);
    curlayer_button = button;

    button = gtk_toggle_button_new_with_label("");
    
    gtk_grid_attach(layer_grid, button, 0, 1, 1, 1);
    gtk_widget_show(button);
    g_signal_connect(button, "clicked", G_CALLBACK(hide_show_layer), index);
*/
    add_grid_buttons();


    // tools signal
    g_signal_connect(play_button, "clicked", G_CALLBACK(play_animation), NULL);

    g_signal_connect(pen_button, "clicked", G_CALLBACK(select_pen), NULL);
    g_signal_connect(eraser_button, "clicked", G_CALLBACK(select_eraser), NULL);
    g_signal_connect(fill_button, "clicked", G_CALLBACK(select_fill), NULL);
    g_signal_connect(line_button, "clicked", G_CALLBACK(select_line), NULL);
    g_signal_connect(rectangle_button, "clicked", G_CALLBACK(select_rectangle), NULL);
    g_signal_connect(circle_button, "clicked", G_CALLBACK(select_circle), NULL);

    //      Drawing signals
    setup_drawing(drawing_area, color_select);

    w = gtk_widget_get_allocated_width((GtkWidget *)drawing_area);
    h = gtk_widget_get_allocated_height((GtkWidget *)drawing_area);

    g_signal_connect(drawing_area, "draw", G_CALLBACK(on_drawingarea_draw), NULL);

    //connects the menu items
    g_signal_connect(export_button_img, "activate", G_CALLBACK(on_export), window);
    g_signal_connect(export_picsel_button, "activate", G_CALLBACK(on_export_picsel), window);
    g_signal_connect(export_button_sprite, "activate", G_CALLBACK(on_export_sprite), window);
    g_signal_connect(export_button_gif, "activate", G_CALLBACK(on_export_gif), window);
    g_signal_connect(open_item, "activate", G_CALLBACK(on_import), window);



    //open_dialog(window);
    
    // Runs the main loop
    gtk_main();

    return 0;
}
