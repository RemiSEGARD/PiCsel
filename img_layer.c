#include "SDL/SDL.h"
#include "SDL/SDL_image.h"
#include "img_layer.h"
#include <err.h>
#include <stdlib.h>
#include <gtk/gtk.h>
static void create_background(SDL_Surface *s)
{
    SDL_Rect rect;
    rect.w = 16;
    rect.h = 16;
    Uint32 p1 = SDL_MapRGBA(s->format, 0x80, 0x80, 0x80, 0xff);
    Uint32 p2 = SDL_MapRGBA(s->format, 0xc0, 0xc0, 0xc0, 0xff);
    for (rect.x = 0; rect.x < s->w; rect.x += 16)
    {
        if (s->w < rect.x + 16)
            rect.w = s->w - rect.x;
        for (rect. y = 0; rect.y < s->h; rect.y += 16)
        {
            if (s->h < rect.y + 16)
                rect.h = s->h - rect.y;
            if ((rect.x / 16 + rect.y / 16) % 2 == 0)
                SDL_FillRect(s, &rect, p1);
            else
                SDL_FillRect(s, &rect, p2);
        }
        rect.h = 16;
    }
}

// Creates a sentinel for linked list layer
Layer* init_layer(int w, int h)
{
    static SDL_Surface *bg = NULL;
    if (bg == NULL)
    {
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

        bg = SDL_CreateRGBSurface(0, w, h, 32,
                rmask, gmask, bmask, amask);
        if (bg == NULL) {
            errx(1, "SDL_CreateRGBSurface() failed");
        }

        create_background(bg);
    }

    Layer *sentinel = malloc(sizeof(Layer));
    sentinel->prev = NULL;
    sentinel->next = NULL;
    sentinel->shown = 1;
    sentinel->index = -1;
    sentinel->img = bg;
    
    add_layer(sentinel, w, h);

    return sentinel;
}

// Add an empty layer to the layer list
void add_layer(Layer *list, int w, int h)
{
    while(list->next != NULL)
    {
        list = list->next;
    }
    Layer *new = malloc(sizeof(Layer));
    list->next = new;
    new->prev = list;
    new->next = NULL;
    new->shown = 1;
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

    new->img = SDL_CreateRGBSurface(0, w, h, 32,
                                   rmask, gmask, bmask, amask);
    if (new->img == NULL) {
        errx(1, "SDL_CreateRGBSurface() failed");
    }
    SDL_FillRect(new->img, NULL, 0x00000000);
}

// Remove the i-th layer from the list
void rm_layer(Layer *list, int index)
{
    Layer *tmp = list;
    while (index > 0 && tmp->next != NULL)
    {
        index--;
        tmp = tmp->next;
    }
    if (index > 0)
        errx(EXIT_FAILURE, "index out of bound");
    Layer *stock = tmp;
    while (tmp->next != NULL)
    {
        tmp->index--;
        tmp = tmp->next;
    }
    Layer *prev = stock->prev;
    prev->next = stock->next;
    free(stock);
    SDL_FreeSurface(stock->img);
}

/*
// Swap the i-th layer with the j-th layer from the list
void swap_layer(Layer *list, int i, int j)
{
    Layer *layer_i = list;
    Layer *layer_j = list;
    while (i > 0 && layer_i->next != NULL)
    {
        i--;
        layer_i = layer_i->next;
    }
    while (j > 0 && layer_j->next != NULL)
    {
        j--;
        layer_j = layer_j->next;
    }
    // swap index
    int index = layer_i->index;
    layer_i->index = layer_j->index;
    layer_j->index = index;
    // swap prev
    Layer *tmp = layer_i->prev;
    layer_i->prev = layer_j->prev;
    layer_j->prev = tmp;
    tmp->next = layer_j
    // swap next
    tmp = layer_i->next;
    layer_i->next = layer_j->next;
    layer_j->next = tmp;
    // renew the prev and next of layers next to layer_i and layer_j
    tmp = layer_i->prev;
    tmp->next = layer_i;
    tmp = layer_i->next;
    if (tmp != NULL)
        tmp->prev = layer_i;
    tmp = layer_j->prev;
    tmp->next = layer_j;
    tmp = layer_j->next;
    if (tmp != NULL)
        tmp->prev = layer_j;
}
*/

// Returns the length of the layer list
size_t length_layer(Layer *list)
{
    size_t len = 0;
    while(list->next != NULL)
    {
        len++;
        list = list->next;
    }
    return len;
}

