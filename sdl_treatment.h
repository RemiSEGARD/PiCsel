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
    SDL_Surface *current;
    int nblayer;
} SDLdata;

GdkRectangle calculate_coord(int x, int y, int win_x);

void main_sdl(int width, int height);

#endif
