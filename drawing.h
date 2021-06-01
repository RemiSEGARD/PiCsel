#include <SDL.h>

#ifndef DRAWING_H
#define DRAWING_H

void set_pen();

void set_select();

void set_eraser();

void set_fill();

void set_line();

void set_rectangle();

void set_circle();

void deselect();

void redraw_surface(GtkDrawingArea *drawing_area, SDL_Surface *surface);

void draw_background();

void setup_drawing(GtkDrawingArea *drawing_area, GtkColorChooser *color, GtkWindow *window);

#endif
