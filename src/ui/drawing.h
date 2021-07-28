#ifndef DRAWING_H
#define DRAWING_H

#include <SDL.h>

typedef enum Tools
{
    DRAW,
    ERASER,
    FILL,
    LINE,
    RECTANGLE,
    CIRCLE,
    SELECT,
} Tools;

gdouble x1,y1;

void set_tool(Tools selected_tool);

void deselect();

void redraw_surface(GtkDrawingArea *drawing_area, SDL_Surface *surface);

void draw_background();

void setup_drawing(GtkDrawingArea *drawing_area, GtkColorChooser *color, GtkWindow *window);

#endif
