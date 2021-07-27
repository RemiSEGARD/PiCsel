#ifndef SDL_TREATMENT_H
#define SDL_TREATMENT_H
#include "SDL/SDL.h"
#include "SDL/SDL_image.h"
#include "sdl_treatment.h"
#include "img_layer.h"
#include "img_frame.h"
#include "gtk/gtk.h"
#include "../file_io/fileio_picsel.h"

/*
typedef struct SDL_data
{
    int width;
    int height;
    Frame *frames;
    Layer *current;
    SDL_Surface *previs;
    int nblayer;
    int nbframe;
    int curframe;
    int curlayer;
} SDLdata;
*/

struct SDL_data sdl_data;

Uint32 get_pixel(SDL_Surface *surface, int x, int y);

void put_pixel(SDL_Surface *surface, int x, int y, Uint32 pixel);

Frame *get_frame(int i);

SDL_Surface *compress_frame(int i, int keep_bg);

void export_current_frame(char *filename);

void export_current_gif(char *filename);

void export_sprite(char *filename);

GdkRectangle calculate_coord(int x, int y, int win_x, int win_y, GdkRGBA* color);

SDL_Surface *previs_select(int x1, int y1, int x2,int y2);

SDL_Surface *previsualisation(void (*fun) (int, int, int, int, int, int, GdkRGBA *),
         int x1, int y1, int x2,int y2,int win_x,int win_y, GdkRGBA* color);

struct SDL_data *get_sdl_data();

void main_sdl(int width, int height, char *filename);

#endif
