#include "img_layer.h"
#include "img_frame.h"
#include "SDL/SDL.h"
#include "SDL/SDL_image.h"
#include <err.h>
#include <stdlib.h>

// Creates the sentinel for the frame list
Frame *init_frame()
{
    Frame *sentinel = malloc(sizeof(Frame));
    sentinel->prev = NULL;
    sentinel->next = NULL;
    sentinel->layer = NULL;
    sentinel->duration = 0.0;
    sentinel->index = -1;
    sentinel->img = NULL;
    add_frame(sentinel);
    add_layer_to_all_frames(sentinel);
    return sentinel;
}

// Add a frame to the list
void add_frame(Frame *list)
{
    while(list->next != NULL)
    {
        list = list->next;
    }
    Frame *new = malloc(sizeof(Frame));
    list->next = new;
    new->next = NULL;
    new->prev = list;
    new->layer = init_layer();
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

    new->img = SDL_CreateRGBSurface(0, 10, 10, 32,
                                   rmask, gmask, bmask, amask);
    if (new->img == NULL) {
        errx(1, "SDL_CreateRGBSurface() failed");
        exit(1);
    }


}

// Add a layer to all frame
void add_layer_to_all_frames(Frame *list)
{
    list = list->next;
    while(list != NULL)
    {
        add_layer(list->layer);
        list = list->next;
    }
}

// Remove the i-th frame of the linked list
void rm_frame(Frame *list, int i)
{
    // TODO
}

// Swap the i-th frame with the j-th frame
void swap_frame(Frame *list, int i, int j)
{
    // TODO
}

// Returns the length of the list
size_t length_frame(Frame *list)
{
    // TODO
}

// Returns the number of layer
size_t nb_layer(Frame *list)
{
    // TODO
}

// Merges all the layers of a frame
void get_image(Frame *frame)
{
    // TODO
}
