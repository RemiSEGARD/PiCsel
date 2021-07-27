#include <gtk/gtk.h>
#include <stdio.h>
#include "../sdl/sdl_treatment.h"
#include "../sdl/img_frame.h"
#include "../sdl/img_layer.h"
#include "ui.h"
#include "drawing.h"
#include <unistd.h>

GtkWidget *curframe_button;
GtkWidget *curlayer_button;
GtkGrid* f_grid;
GtkGrid* l_grid;


void select_layer(int frame, int layer)
{
    int iframe = frame;
    int ilayer = layer;

    Frame *frames = sdl_data.frames->next;
    while (iframe > 0 && frames != NULL)
    {
        iframe--;
        frames = frames->next;
    }
    Layer *nextlayer = frames->layer->next;
    while (ilayer > 0 && nextlayer != NULL)
    {
        ilayer--;
        nextlayer = nextlayer->next;
    }
    if (nextlayer != NULL)
    {
        sdl_data.current = nextlayer;
        sdl_data.curlayer = layer;
        sdl_data.curframe = frame;
    }
}

static void next_frame()
{
    if(sdl_data.curframe < sdl_data.nbframe - 1)
    {
        // no need to change curframe, select_layer does it
        select_layer(sdl_data.curframe+1, sdl_data.curlayer);
    }
    else
    {
        select_layer(0, sdl_data.curlayer);
    }
}

static void new_frame()
{
    add_frame(sdl_data.frames, sdl_data.width, sdl_data.height, sdl_data.nblayer);
    sdl_data.nbframe++;
}

static void new_layer()
{
    add_layer_to_all_frames(sdl_data.frames, sdl_data.width, sdl_data.height);
    sdl_data.nblayer++;
}

static void choose_frame(GtkWidget* widget, gpointer data)
{
    deselect();
    (void) widget;
    select_layer(*((int *)data), sdl_data.curlayer);
    SDL_Surface *surface = compress_frame(-1, 1);
    redraw_surface(darea, surface);
    gtk_widget_set_sensitive(curframe_button, TRUE);
    gtk_widget_set_sensitive(widget, FALSE);
    curframe_button = widget;
    
}

static void on_next_frame()
{
    deselect();
    next_frame();
    SDL_Surface *surface = compress_frame(-1, 1);
    redraw_surface(darea, surface);

}

static void on_new_frame(GtkWidget *widget, gpointer data)
{
    (void) widget;
    char label[15];
    sprintf(label, "%u", sdl_data.nbframe + 1);
    GtkWidget *button = gtk_button_new_with_label(label);
    
    new_frame();
    gtk_grid_insert_column(data, sdl_data.nbframe);
    gtk_grid_attach(data, button, sdl_data.nbframe, 0, 1, 1);
    gtk_widget_show(button);
    int *index = (int *)malloc(sizeof(int));
    *index = sdl_data.nbframe - 1;
    g_signal_connect(button, "clicked", G_CALLBACK(choose_frame), index);
}

static void choose_layer(GtkWidget *widget, gpointer data)
{
    (void) widget;
    select_layer(sdl_data.curframe, *((int *)data));
    SDL_Surface *surface = compress_frame(-1, 1);
    redraw_surface(darea, surface);
    gtk_widget_set_sensitive(curlayer_button, TRUE);
    gtk_widget_set_sensitive(widget, FALSE);
    curlayer_button = widget;
}

static void hide_show_layer(GtkWidget *widget, gpointer data)
{
    (void) widget;
    Frame *f = sdl_data.frames->next;
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

static void on_new_layer(GtkWidget *widget, gpointer data)
{
    (void) widget;
    char label[15];
    sprintf(label, "%u", sdl_data.nblayer + 1);
    GtkWidget *button = gtk_button_new_with_label(label);
    
    new_layer();
    gtk_grid_insert_column(data, sdl_data.nblayer);
    gtk_grid_attach(data, button, sdl_data.nblayer, 0, 1, 1);
    gtk_widget_show(button);
    int *index = (int *) malloc(sizeof(int));
    *index = sdl_data.nblayer - 1;
    g_signal_connect(button, "clicked", G_CALLBACK(choose_layer), index);
    
    button = gtk_toggle_button_new_with_label("");
    
    gtk_grid_attach(data, button, sdl_data.nblayer, 1, 1, 1);
    gtk_widget_show(button);
    g_signal_connect(button, "clicked", G_CALLBACK(hide_show_layer), index);
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

    char label[15];
    for (int i = 0; i < sdl_data.nbframe - 1; i++)
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
    for (int i = 0; i < sdl_data.nblayer - 1; i++)
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

void reset_grids()
{
    for (int i = 0; i <= sdl_data.nbframe; i++)
    {
        gtk_grid_remove_column(f_grid, 0);
    }
    for (int i = 0; i <= sdl_data.nblayer; i++)
    {
        gtk_grid_remove_column(l_grid, 0);
    }
}

int id = 0;

static gboolean play(gpointer data)
{
    (void) data;
    on_next_frame();
    return TRUE;
}

static void play_animation()
{
    if (id == 0)
        id = g_timeout_add(get_frame(sdl_data.curframe)->duration, play, NULL);
    else
    {
        g_source_remove(id);
        id = 0;
        gtk_button_clicked((GtkButton *)curframe_button);
    }
}

void setup_nav(GtkBuilder *builder)
{
    GtkButton* play_button = GTK_BUTTON(gtk_builder_get_object(builder, "play"));
    GtkButton* add_frame = GTK_BUTTON(gtk_builder_get_object(builder, "add_frame"));
    GtkButton* add_layer = GTK_BUTTON(gtk_builder_get_object(builder, "add_layer"));
    GtkGrid* frame_grid = GTK_GRID(gtk_builder_get_object(builder, "frame_grid"));
    GtkGrid* layer_grid = GTK_GRID(gtk_builder_get_object(builder, "layer_grid"));

    f_grid = frame_grid;
    l_grid = layer_grid;

    add_grid_buttons();

    g_signal_connect(add_frame, "clicked", G_CALLBACK(on_new_frame), frame_grid);
    g_signal_connect(add_layer, "clicked", G_CALLBACK(on_new_layer), layer_grid);
    g_signal_connect(play_button, "clicked", G_CALLBACK(play_animation), NULL);
}
