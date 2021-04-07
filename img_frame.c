#include "img_layer.h"
#include "img_frame.h"
#include "SDL/SDL.h"
#include "SDL/SDL_image.h"
#include <err.h>
#include <stdlib.h>

// Creates the sentinel for the frame list
Frame *init_frame(int w, int h)
{
    Frame *sentinel = malloc(sizeof(Frame));
    sentinel->prev = NULL;
    sentinel->next = NULL;
    sentinel->layer = NULL;
    sentinel->duration = 0.0;
    sentinel->index = -1;
    sentinel->img = NULL;
    add_frame(sentinel, w , h, 1);
    return sentinel;
}

// Add a frame to the list contenaing a given number of layer
void add_frame(Frame *list, int w, int h, int nblayer)
{
    while(list->next != NULL)
    {
        list = list->next;
    }
    Frame *new = malloc(sizeof(Frame));
    list->next = new;
    new->next = NULL;
    new->prev = list;
    new->layer = init_layer(w, h);
    for (int i = 0; i < nblayer - 1; i++)
    {
        add_layer(new->layer, w, h);
    }
    new->index = list->index + 1;
    new->duration = 0.0;
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
        exit(1);
    }


}

// Add a layer to all frame
void add_layer_to_all_frames(Frame *list, int w, int h)
{
    list = list->next;
    while(list != NULL)
    {
        add_layer(list->layer, w, h);
        list = list->next;
    }
}

// Remove the i-th frame of the linked list
void rm_frame(Frame *list, int i)
{
    Frame *tmp = list;
    while (i > 0 && tmp->next != NULL)
    {
        i--;
        tmp = tmp->next;
    }
    if (i > 0)
        errx(EXIT_FAILURE, "index out of bound");
    Frame *stock = tmp;
    while (tmp->next != NULL)
    {
        tmp->index--;
        tmp = tmp->next;
    }
    Frame *prev = stock->prev;
    prev->next = stock->next;
    free(stock);
    SDL_FreeSurface(stock->img);

}

// Swap the i-th frame with the j-th frame
void swap_frame(Frame *list, int i, int j)
{
    Frame *frame_i = list;
    Frame *frame_j = list;
    while (i > 0 && frame_i->next != NULL)
    {
        i--;
        frame_i = frame_i->next;
    }
    while (j > 0 && frame_j->next != NULL)
    {
        j--;
        frame_j = frame_j->next;
    }
    // swap index
    int index = frame_i->index;
    frame_i->index = frame_j->index;
    frame_j->index = index;
    // swap prev
    Frame *tmp = frame_i->prev;
    frame_i->prev = frame_j->prev;
    frame_j->prev = tmp;
    tmp->next = frame_j;
    // swap next
    tmp = frame_i->next;
    frame_i->next = frame_j->next;
    frame_j->next = tmp;
    // renew the prev and next of layers next to layer_i and layer_j
    tmp = frame_i->prev;
    tmp->next = frame_i;
    tmp = frame_i->next;
    if (tmp != NULL)
        tmp->prev = frame_i;
    tmp = frame_j->prev;
    tmp->next = frame_j;
    tmp = frame_j->next;
    if (tmp != NULL)
        tmp->prev = frame_j;
}

// Returns the length of the list
size_t length_frame(Frame *list)
{
    size_t len = 0;
    while(list->next != NULL)
    {
        len++;
        list = list->next;
    }
    return len;

}

// Returns the number of layer
size_t nb_layer(Frame *list)
{
    Layer* layer = list->next->layer;
    size_t len = 0;
    while (layer != NULL)
    {
        layer = layer->next;
        len++;
    }
    return len;
}

// Merges all the layers of a frame
void get_image(Frame *frame)
{
    // TODO
    Layer *layer = frame->layer;
    while(layer != NULL)
    {
        layer = layer->next;
    }
}
