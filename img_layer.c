#include "SDL/SDL.h"
#include "SDL/SDL_image.h"
#include "img_layer.h"
#include <err.h>
#include <stdlib.h>


// Creates a sentinel for linked list layer
Layer* init_layer()
{
    Layer *sentinel = malloc(sizeof(Layer));
    sentinel->prev = NULL;
    sentinel->next = NULL;
    sentinel->index = -1;
    sentinel->img = NULL;
    
    add_layer(sentinel);

    return sentinel;
}

// Add an empty layer to the layer list
void add_layer(Layer *list)
{
    while(list->next != NULL)
    {
        list = list->next;
    }
    Layer *new = malloc(sizeof(Layer));
    list->next = new;
    new->prev = list;
    new->next = NULL;
    new->index = list->index + 1;

    Uint32 rmask, gmask, bmask, amask;

    /* SDL interprets each pixel as a 32-bit number, so our masks must depend
       on the endianness (byte order) of the machine */
#if SDL_BYTEORDER == SDL_BIG_ENDIAN
    rmask = 0xff000000;
    gmask = 0x00ff0000;
    bmask = 0x0000ff00;
    amask = 0x000000ff;
#else
    rmask = 0x000000ff;
    gmask = 0x0000ff00;
    bmask = 0x00ff0000;
    amask = 0xff000000;
#endif

    new->img = SDL_CreateRGBSurface(0, 10, 10, 32,
                                   rmask, gmask, bmask, amask);
    if (new->img == NULL) {
        errx(1, "SDL_CreateRGBSurface() failed");
    }
}

// Remove the i-th layer from the list
void rm_layer(Layer *list, int index)
{
    // TODO
}

// Swap the i-th layer with the j-th layer from the list
void swap_layer(Layer *list, int i, int j)
{
    // TODO
}


// Returns the length of the layer list
size_t length_layer(Layer *list)
{
    // TODO
}

