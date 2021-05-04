#include <SDL.h>

#ifndef DRAWING_H
#define DRAWING_H

void redraw_surface(GtkDrawingArea *drawing_area, SDL_Surface *surface);

void draw_background();

void setup_drawing(GtkDrawingArea *drawing_area, GtkColorChooser *color);


#endif
