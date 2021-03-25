#include <stdlib.h>
#include <SDL.h>
#include <err.h>
#include <gtk/gtk.h>

//const size_t PIXEL_SIZE = 50;
/*
    static inline
Uint8* pixel_ref(SDL_Surface *surf, unsigned x, unsigned y)
{
    // Function from epita prog S3 site
    int bpp = surf->format->BytesPerPixel;
    return (Uint8*)surf->pixel + y * surf->pitch + x * bpp;
}

Uint32 get_pixel(SDL_Surface *surface, unsigned x, unsigned y)
{
    // Function from epita prog S3 site
    Uint8 *p = pixel_ref(surface, x, y);

    swicth (surface->format->BytesPerPixel)
    {
        case 1:
            return *p;

        case 2:
            return *(Uint16 *)p;

        case 3:
            if(SDL_BYTEORDER == SDL_BIG_ENDIAN)
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

    swicth(surface->format->BytesPerPixel)
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
                p[2] = (pixel >> 16 & 0xff;)
            }
            break;

        case 4:
            *(Uint32 *)p = pixel;
            break;
    }
}

static void update_surface(SDL_Surface* screen, SDL_Surface* image)
{
    // Check error
    if (SDL_BlitSurface(image, NULL, screen, NULL) < 0)
        warnx("BlitSurface error: %s\n", SDL_GetError());

    SDL_UpdateRect(screen, 0, 0, image->w, image->h);
}

static void paint_sdl(SDL_Surface* image, unsigned x, unsigned y)
{
    for (size_t i = 0; i < PIXEL_SIZE; i++)
    {
        for (size_t j = 0; j < PIXEL_SIZE; j++)
        {
            // newPixel is set to the color black for now
            Uint32 newPixel = SDL_MapRGB(image_surface->format, 0, 0, 0);
            put_pixel(image, x + i, y + j, newPixel);
        }
    }
}
*/
int sx = 200;
//int sy = 50;

/* Compute the position to change the pixel in SDL from the coords of the click
 *  in the GtkDrawingArea
 * x = position of the click
 * y = position of the click
 * win_x = width of the Drawing Area
 * Returns a GdkRectangle storing the position and size of the pixel to draw
 *  on the window
 */
GdkRectangle calculate_coord(int x, int y, int win_x)
{
    GdkRectangle rect;
    rect.x = x - x % (win_x / sx);
    rect.y = y - y % (win_x / sx);
    rect.width = win_x / sx;
    rect.height = win_x / sx;
    return rect;
}
/*
int main_sdl()
{
    // Creates a new SDL_Surface
    // SDL_Surface* image_surface = SDL_CreateRGBSurface(0, width, height, 32,
    //        0, 0, 0, 0);

    // The rest is for display purpose
    //SDL_Surface* screen_surface;
    //init_sdl();

    //screen_surface = display_image(image_surface);

    //update_surface(screen_surface, image_surface);

    //SDL_FreeSurface(image_surface);
}*/
