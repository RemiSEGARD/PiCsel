#ifndef SELECTION_H
#define SELECTION_H

#include <gtk/gtk.h>

SDL_Rect select_pos;
SDL_Rect selecttmp_pos;
SDL_Rect clipboard_pos;

int id_to;

gboolean glow(gpointer data);

void deselect();

void draw_glow(gpointer data);

void blit_selects();

void selection_press(int x, int y, int win_x, int win_y, GtkWidget *widget);

void selection_motion(int x, int y, int win_x, int win_y, GtkWidget *widget);

void selection_release(int x, int y, int win_x, int win_y, GtkWidget *widget);

void delete_selection();

void copy_selection();

void cut_selection();

void paste(gpointer data);

void select_all(gpointer data);

#endif
