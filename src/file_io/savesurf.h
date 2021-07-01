#ifndef SAVESURF_H
#define SAVESURF_H

#include <stdlib.h>
#include <string.h>
#include <SDL/SDL.h>
#include <SDL/SDL_image.h>
#include <png.h>

int png_save_surface(char *filename, SDL_Surface *surf);

#endif
