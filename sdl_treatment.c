#include <stdlib.h>
#include <SDL.h>
#include <err.h>
#include <gtk/gtk.h> 
#include "sdl_treatment.h"
#include "img_layer.h"
#include "img_frame.h"

struct SDL_data sdl_data;

void select_layer(int frame, int layer)
{
    int iframe = frame;
    int ilayer = layer;

    Frame *frames = sdl_data.frames->next;
    while (iframe > 0 && frames != NULL)
    {
        iframe--;
        frames = frames->next;
    }
    Layer *nextlayer = frames->layer->next;
    while (ilayer > 0 && nextlayer != NULL)
    {
        ilayer--;
        nextlayer = nextlayer->next;
    }
    if (nextlayer != NULL)
    {
        sdl_data.current = nextlayer->img;
        sdl_data.curlayer = layer;
        sdl_data.curframe = frame;
    }
}

static inline Uint8 *pixel_ref(SDL_Surface *surf, unsigned x, unsigned y)
{
    // Function from epita prog S3 site
    int bpp = surf->format->BytesPerPixel;
    return (Uint8 *)surf->pixels + y * surf->pitch + x * bpp;
}

Uint32 get_pixel(SDL_Surface *surface, unsigned x, unsigned y)
{
    // Function from epita prog S3 site
    Uint8 *p = pixel_ref(surface, x, y);

    switch (surface->format->BytesPerPixel)
    {
    case 1:
        return *p;

    case 2:
        return *(Uint16 *)p;

    case 3:
        if (SDL_BYTEORDER == SDL_BIG_ENDIAN)
            return p[0] << 16 | p[1] << 8 | p[2];
        else
            return p[0] | p[1] << 8 | p[2] << 16;

    case 4:
        return *(Uint32 *)p;
    }

    return 0;
}

static void put_pixel(SDL_Surface *surface, unsigned x, unsigned y, Uint32 pixel)
{
    // Function from epita prog S3 site
    Uint8 *p = pixel_ref(surface, x, y);

    switch(surface->format->BytesPerPixel)
    {
        case 1:
            *p = pixel;
            break;

        case 2:
            *(Uint16 *)p = pixel;
            break;

        case 3:
            if (SDL_BYTEORDER == SDL_BIG_ENDIAN)
            {
                p[0] = (pixel >> 16) & 0xff;
                p[1] = (pixel >> 8) & 0xff;
                p[2] = pixel & 0xff;
            }
            else
            {
                p[0] = pixel & 0xff;
                p[1] = (pixel >> 8) & 0xff;
                p[2] = (pixel >> 16) & 0xff;
            }
            break;

        case 4:
            *(Uint32 *)p = pixel;
            break;
    }
}

/*
static void update_surface(SDL_Surface* screen, SDL_Surface* image)
{
    // Check error
    if (SDL_BlitSurface(image, NULL, screen, NULL) < 0)
        warnx("BlitSurface error: %s\n", SDL_GetError());

    SDL_UpdateRect(screen, 0, 0, image->w, image->h);
}
*/


/* Compute the position to change the pixel in SDL from the coords of the click
 *  in the GtkDrawingArea
 * x = position of the click
 * y = position of the click
 * win_x = width of the Drawing Area
 * Returns a GdkRectangle storing the position and size of the pixel to draw
 *  on the window
 */

GdkRectangle calculate_coord(int x, int y, int win_x, int win_y)
{
    // gonna need to add pixel in arguments once palette is made
    Uint32 pixel = SDL_MapRGBA(sdl_data.current->img->format, 100, 100, 100, 255);
    GdkRectangle rect;
    if (win_x < win_y)
    {
        rect.x = x - x % (win_x / sdl_data.width);
        rect.y = y - y % (win_x / sdl_data.width);
        rect.width = win_x / sdl_data.width;
        rect.height = win_x / sdl_data.width;
        printf("x = %i\ny = %i\n",x*sdl_data.width/win_x, y*sdl_data.width/win_x);
        int new_x = x * sdl_data.width / win_x;
        int new_y = y * sdl_data.width / win_x;
        if (new_x > 0 && new_x < sdl_data.width && new_y > 0 && new_y < sdl_data.height)
        {
            put_pixel(sdl_data.current->img, new_x, new_y, pixel);
        }
    }
    else
    {
        rect.x = x - x % (win_y / sdl_data.height);
        rect.y = y - y % (win_y / sdl_data.height);
        rect.width = win_y / sdl_data.height;
        rect.height = win_y / sdl_data.height;
        printf("x = %i\ny = %i\n",x*sdl_data.height/win_y, y*sdl_data.height/win_y);
        int new_x = x * sdl_data.height / win_y;
        int new_y = y * sdl_data.height / win_y;
        if (new_x > 0 && new_x < sdl_data.width && new_y > 0 && new_y < sdl_data.height)
        {
            put_pixel(sdl_data.current->img, new_x, new_y, pixel);
        }
    }
    return rect;
}

void main_sdl(int width, int height)
{
    // Creates a new SDL_Surface
    // SDL_Surface* image_surface = SDL_CreateRGBSurface(0, width, height, 32,
    //        0, 0, 0, 0);
    // TODO
    sdl_data.width = width;
    sdl_data.height = height;
    sdl_data.frames = init_frame(width, height);
    sdl_data.current = sdl_data.frames->next->layer->next;
    sdl_data.nblayer = 1;
    sdl_data.curlayer = 0;
    sdl_data.curframe = 0;
    // import given file
    // draw given file
    //put_pixel(sdl_data.current->img, 0, 0, SDL_MapRGBA(sdl_data.current->img->format, 100, 100, 100, 255));
    //printf("%lu hi\n", sdl_data.current->img);
    //printf("hi\n");
    // display
    /*sdl_init();
    SDL_Surface* screen_surface = display_image(sdl_data.current->img);
    SDL_FreeSurface(screen_surface);*/
}
