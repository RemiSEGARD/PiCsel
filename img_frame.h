#ifndef IMG_FRAME_H
#define IMG_FRAME_H

#include "img_layer.h"
#include "SDL/SDL.h"
#include "SDL/SDL_image.h"

typedef struct Frame
{
    struct Frame *next;
    struct Frame *prev;
    Layer *layer;
    int index;
    double duration;
    SDL_Surface *img;
} Frame;

// Creates the sentinel for the frame list
Frame *init_frame();

// Add a frame to the list
void add_frame(Frame *list);

// Add a layer to all frame
void add_layer_to_all_frames(Frame *list);

// Remove the i-th frame of the linked list
void rm_frame(Frame *list, int i);

// Swap the i-th frame with the j-th frame
void swap_frame(Frame *list, int i, int j);

// Returns the length of the list
size_t length_frame(Frame *list);

// Returns the number of layer
size_t nb_layer(Frame *list);

// Merges all the layers of a frame
void get_image(Frame *frame);

#endif
