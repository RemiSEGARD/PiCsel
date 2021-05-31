#ifndef FILEIO_PICSEL_H
#define FILEIO_PICSEL_H
#include <stdlib.h>
#include <unistd.h>
#include "img_frame.h"
#include "img_layer.h"
#include "SDL.h"
#include "SDL_image.h"
#include "sdl_treatment.h"

typedef struct SDL_data
{
    int width;
    int height;
    Frame *frames;
    Layer *current;
    SDL_Surface *previs;
    SDL_Surface *select;
    SDL_Surface *clipboard;
    int nblayer;
    int nbframe;
    int curframe;
    int curlayer;
} SDLdata;


void export_picsel(char *filename, SDLdata *sdl_data);

Frame *import_picsel(char *filename);
#endif
