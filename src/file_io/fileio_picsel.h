#ifndef FILEIO_PICSEL_H
#define FILEIO_PICSEL_H
#include <stdlib.h>
#include <unistd.h>
#include "../sdl/img_frame.h"
#include "../sdl/img_layer.h"
#include "SDL.h"
#include "SDL_image.h"
#include "../sdl/sdl_treatment.h"

typedef struct SDL_data
{
    int width;
    int height;
    Frame *frames;
    Layer *current;
    SDL_Surface *previs;
    SDL_Surface *select;
    SDL_Surface *selecttmp;
    SDL_Surface *clipboard;
    int nblayer;
    int nbframe;
    int curframe;
    int curlayer;
} SDLdata;


void export_picsel(char *filename, SDLdata *sdl_data);

Frame *import_picsel(char *filename);
#endif
