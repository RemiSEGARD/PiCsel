#ifndef UI_H
#define UI_H

#include <gtk/gtk.h>

GtkDrawingArea *darea;

int main_ui(int x, int y, char *filename);

void select_pen();

void select_eraser();

void select_fill();

void select_line();

void select_rectangle();

void select_circle();

#endif
