#ifndef IMG_LAYER_H
#define IMG_LAYER_H

#include "SDL/SDL.h"
#include "SDL/SDL_image.h"


typedef struct Layer
{
    struct Layer *next;
    struct Layer *prev;
    SDL_Surface *img;
    int shown;
    int index;
} Layer;

// Creates a sentinel for linked list layer
Layer* init_layer(int w, int h);

// Add an empty layer to the layer list
void add_layer(Layer *list, int w, int h);

// Remove the i-th layer from the list
void rm_layer(Layer *list, int index);

// Swap the i-th layer with the j-th layer from the list
void swap_layer(Layer *list, int i, int j);

// Returns the length of the layer list
size_t length_layer(Layer *list);

#endif
