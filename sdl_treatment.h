#ifndef SDL_TREATMENT_H
#define SDL_TREATMENT_H
#include "SDL/SDL.h"
#include "SDL/SDL_image.h"
#include "sdl_treatment.h"
#include "img_layer.h"
#include "img_frame.h"

typedef struct SDL_data
{
    int width;
    int height;
    Frame *frames;
    Layer *current;
    int nblayer;
    int curframe;
    int curlayer;
} SDLdata;

void export_current_frame(char *filename);

GdkRectangle calculate_coord(int x, int y, int win_x, int win_y);

void main_sdl(int width, int height);

#endif
