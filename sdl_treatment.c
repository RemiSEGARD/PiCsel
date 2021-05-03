#include <stdlib.h>
#include <SDL.h>
#include <SDL_image.h>
#include <err.h>
#include <gtk/gtk.h> 
#include "display_sdl.h"
#include "sdl_treatment.h"
#include "img_layer.h"
#include "img_frame.h"
#include <math.h>

void select_layer(int frame, int layer);

void fill_sdl(int x, int y, GdkRGBA* color, Uint8 match_r, Uint8 match_g,Uint8 match_b,Uint8 match_a);

struct SDL_data sdl_data;

//SDL_Surface *screen_surface;

void prev_frame()
{
    if(sdl_data.curframe != 0)
    {
        select_layer(sdl_data.curframe-1, sdl_data.curlayer);
    }
}

void next_frame()
{
    if(sdl_data.curframe < sdl_data.nbframe - 1)
    {
        select_layer(sdl_data.curframe+1, sdl_data.curlayer);
    }
}

void new_frame()
{
    add_frame(sdl_data.frames, sdl_data.width, sdl_data.height, sdl_data.nblayer);
    sdl_data.nbframe++;
}

void prev_layer()
{
    if(sdl_data.curlayer != 0)
    {
        sdl_data.curframe--;
        sdl_data.current = sdl_data.current->prev;
    }
}

void next_layer()
{
    if(sdl_data.current->next != NULL)
    {
        sdl_data.curframe++;
        sdl_data.current = sdl_data.current->next;
    }
}

void new_layer()
{
    add_layer_to_all_frames(sdl_data.frames, sdl_data.width, sdl_data.height);
    sdl_data.nblayer++;
}


Frame* get_frame(int i)
{
    Frame *res = sdl_data.frames->next;
    while (i > 0 && res != NULL)
    {
        i--;
        res = res->next;
    }
    return res;
}

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
        sdl_data.current = nextlayer;
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

void put_pixel(SDL_Surface *surface, unsigned x, unsigned y, Uint32 pixel)
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

SDL_Surface *compress_frame(int i)
{
    if (i == -1)
        i = sdl_data.curframe;
    Frame *f = get_frame(i);
    Layer *l = f->layer->next;
    while (l != NULL)
    {
        for (int i = 0; i < sdl_data.width; i++)
        {
            for (int j = 0; j < sdl_data.height; j++)
            {
                Uint32 pixel = get_pixel(l->img, i, j);
                put_pixel(f->img, i , j, pixel);
            }
        }
        l = l->next;
    }
    return f->img;
}

void import_img(char *file)
{
    (void)file;
}

void export_current_frame(char *filename)
{
    SDL_Surface *c = compress_frame(sdl_data.curframe);
    SDL_SaveBMP(c, filename);
    //SDL_SaveBMP(sdl_data.current->img, filename);
}


/* Compute the position to change the pixel in SDL from the coords of the click
 *  in the GtkDrawingArea
 * x = position of the click
 * y = position of the click
 * win_x = width of the Drawing Area
 * Returns a GdkRectangle storing the position and size of the pixel to draw
 *  on the window
 */


GdkRectangle calculate_coord(int x, int y, int win_x, int win_y, GdkRGBA* color)
{
    // gonna need to add pixel in arguments once palette is made
    Uint32 pixel = SDL_MapRGBA(sdl_data.current->img->format, color->red * 255, color->green* 255, color->blue * 255, color->alpha*255);
    GdkRectangle rect;
    if (x < 0 || x > win_x || y < 0 || y > win_y)
    {
        rect.x = 0;
        rect.y = 0;
        rect.width = 0;
        rect.height = 0;
        return rect;
    }
    if (win_x < win_y)
    {
        rect.x = x - x % (win_x / sdl_data.width);
        rect.y = y - y % (win_x / sdl_data.width);
        rect.width = win_x / sdl_data.width;
        rect.height = win_x / sdl_data.width;
        if (x >= 0 && y >= 0 
                && y < sdl_data.height * rect.height
                && x < sdl_data.width * rect.width)
        {
            put_pixel(sdl_data.current->img, 
                    x * sdl_data.width / (win_x - win_x % sdl_data.width),
                    y * sdl_data.width / (win_x - win_x % sdl_data.width),
                    pixel);
            return rect;
        }
    }
    else
    {
        rect.x = x - x % (win_y / sdl_data.height);
        rect.y = y - y % (win_y / sdl_data.height);
        rect.width = win_y / sdl_data.height;
        rect.height = win_y / sdl_data.height;
        if (x >= 0 && y >= 0 
                && y < sdl_data.height * rect.height
                && x < sdl_data.width * rect.width)
        {
            put_pixel(sdl_data.current->img,
                    x * sdl_data.height / (win_y - win_y % sdl_data.height),
                    y * sdl_data.height / (win_y - win_y % sdl_data.height),
                    pixel);
            return rect;
        }
    }
    rect.width = 0;
    rect.height = 0;
    return rect;
}

void fill(int x, int y, int win_x, int win_y, GdkRGBA* color)
{
    // function called by drawing.c
    // prepares everything for the actual fill function

    // calculate the new coordinates
    if (win_x < win_y)
    {
        x = x * sdl_data.width / (win_x - win_x % sdl_data.width);
        y = y * sdl_data.width / (win_x - win_x % sdl_data.width);
    }
    else
    {
        x = x * sdl_data.height / (win_y - win_y % sdl_data.height);
        y = y * sdl_data.height / (win_y - win_y % sdl_data.height);
    }
    Uint8 r,g,b,a;
    // gets the color that we want to replace by GdkRGBA* color
    Uint32 pixel = get_pixel(sdl_data.current->img, x, y);
    SDL_GetRGBA(pixel, sdl_data.current->img->format, &r, &g, &b, &a);
    g_print("%d\n",r);
    fill_sdl(x,y,color,r,g,b,a);
}

void fill_sdl(int x, int y, GdkRGBA* color, Uint8 match_r, Uint8 match_g,Uint8 match_b,Uint8 match_a)
{
    // Fills on the SDL_Surface
    // match_r, match_g, match_b, match_a represent the rgba values needed to match the color
    if (x < 0 || y < 0 || x >= sdl_data.width || y >= sdl_data.height)
        return;
    Uint8 r,g,b,a;
    Uint32 pixel = get_pixel(sdl_data.current->img, x, y);
    SDL_GetRGBA(pixel, sdl_data.current->img->format, &r, &g, &b, &a);
    
    // condition needs to be changed to detect the proper pixel now it only detects white pixels
    if (r == 255 && g == 255 && b == 255)
    {
        // converts the color from GdkRGBA to a value from 0 to 255
        Uint32 p = SDL_MapRGBA(sdl_data.current->img->format, color->red * 255, color->green* 255, color->blue * 255, color->alpha*255);
        put_pixel(sdl_data.current->img, x, y, p);
        // launches all the recursive function to draw the pixels around current pixel
        fill_sdl(x+1,y,color,match_r,match_g,match_b,match_a);
        fill_sdl(x-1,y,color,match_r,match_g,match_b,match_a);
        fill_sdl(x,y+1,color,match_r,match_g,match_b,match_a);
        fill_sdl(x,y-1,color,match_r,match_g,match_b,match_a);
    }
}


void line(int x1, int y1, int x2, int y2,int win_x,int win_y, GdkRGBA* color)
{
    // TODO
    // Draws the line from (x1,y1) to (x2,y2) onthe SDL_Surface
    if (win_x < win_y)
    {
        x1 = x1 * sdl_data.width / (win_x - win_x % sdl_data.width);
        y1 = y1 * sdl_data.width / (win_x - win_x % sdl_data.width);
        x2 = x2 * sdl_data.width / (win_x - win_x % sdl_data.width);
        y2 = y2 * sdl_data.width / (win_x - win_x % sdl_data.width);
    }
    else
    {
        x1 = x1 * sdl_data.height / (win_y - win_y % sdl_data.height);
        y1 = y1 * sdl_data.height / (win_y - win_y % sdl_data.height);
        x2 = x2 * sdl_data.height / (win_y - win_y % sdl_data.height);
        y2 = y2 * sdl_data.height / (win_y - win_y % sdl_data.height);
    }
    // Calculate coefficient
    int alpha = (y2-y1)/(x2-x1);
    // Convert GdkRGBA color to a value usable by SDL
    Uint32 p = SDL_MapRGBA(sdl_data.current->img->format, color->red * 255, color->green* 255, color->blue * 255, color->alpha*255);
    // Draws the line
}

void rectangle(int x1, int y1, int x2, int y2, int win_x, int win_y, GdkRGBA* color)
{
    // Draws a rectangle from (x1,y1) to (x2,y2) on the SDL_Surface
    // Calculate the new coordinates
    if (win_x < win_y)
    {
        x1 = x1 * sdl_data.width / (win_x - win_x % sdl_data.width);
        y1 = y1 * sdl_data.width / (win_x - win_x % sdl_data.width);
        x2 = x2 * sdl_data.width / (win_x - win_x % sdl_data.width);
        y2 = y2 * sdl_data.width / (win_x - win_x % sdl_data.width);
    }
    else
    {
        x1 = x1 * sdl_data.height / (win_y - win_y % sdl_data.height);
        y1 = y1 * sdl_data.height / (win_y - win_y % sdl_data.height);
        x2 = x2 * sdl_data.height / (win_y - win_y % sdl_data.height);
        y2 = y2 * sdl_data.height / (win_y - win_y % sdl_data.height);
    }
    // case 1
    if (x1 < x2 && y1 < y2)
    {
        // Gets height and width of rectangle
        int h = y2 - y1;
        int w = x2 - x1;
        // Convert GdkRGBA color to a value usable by SDL
        Uint32 pixel = SDL_MapRGBA(sdl_data.current->img->format, color->red * 255, color->green* 255, color->blue * 255, color->alpha*255);
        // Draws the horizontal lines
        for (int i = 0; i < w; i++)
        {
            put_pixel(sdl_data.current->img, x1 + i, y1, pixel);
            put_pixel(sdl_data.current->img, x1 + i, y1 + h, pixel);
        }
        // Draws the verticle lines
        for (int i = 0; i <= h; i++)
        {
            put_pixel(sdl_data.current->img, x1, y1 + i, pixel);
            put_pixel(sdl_data.current->img, x1 + w, y1 + i, pixel);
        }
    }
    // case 2
    if (x1 > x2 && y1 > y2)
    {
        // Gets height and width of rectangle
        int h = y1 - y2;
        int w = x1 - x2;
        // Convert GdkRGBA color to a value usable by SDL
        Uint32 pixel = SDL_MapRGBA(sdl_data.current->img->format, color->red * 255, color->green* 255, color->blue * 255, color->alpha*255);
        // Draws the horizontal lines
        for (int i = 0; i < w; i++)
        {
            put_pixel(sdl_data.current->img, x1 - i, y1, pixel);
            put_pixel(sdl_data.current->img, x1 - i, y1 - h, pixel);
        }
        // Draws the verticle lines
        for (int i = 0; i <= h; i++)
        {
            put_pixel(sdl_data.current->img, x1, y1 - i, pixel);
            put_pixel(sdl_data.current->img, x1 - w, y1 - i, pixel);
        }
    }
    // case 3
    if (x1 < x2 && y1 > y2)
    {
        // Gets height and width of rectangle
        int h = y1 - y2;
        int w = x2 - x1;
        // Convert GdkRGBA color to a value usable by SDL
        Uint32 pixel = SDL_MapRGBA(sdl_data.current->img->format, color->red * 255, color->green* 255, color->blue * 255, color->alpha*255);
        // Draws the horizontal lines
        for (int i = 0; i < w; i++)
        {
            put_pixel(sdl_data.current->img, x1 + i, y1, pixel);
            put_pixel(sdl_data.current->img, x1 + i, y1 - h, pixel);
        }
        // Draws the verticle lines
        for (int i = 0; i <= h; i++)
        {
            put_pixel(sdl_data.current->img, x1, y1 - i, pixel);
            put_pixel(sdl_data.current->img, x1 + w, y1 - i, pixel);
        }
    }
    // case 4
    if (x1 > x2 && y1 < y2)
    {
        // Gets height and width of rectangle
        int h = y2 - y1;
        int w = x1 - x2;
        // Convert GdkRGBA color to a value usable by SDL
        Uint32 pixel = SDL_MapRGBA(sdl_data.current->img->format, color->red * 255, color->green* 255, color->blue * 255, color->alpha*255);
        // Draws the horizontal lines
        for (int i = 0; i < w; i++)
        {
            put_pixel(sdl_data.current->img, x1 - i, y1, pixel);
            put_pixel(sdl_data.current->img, x1 - i, y1 + h, pixel);
        }
        // Draws the verticle lines
        for (int i = 0; i <= h; i++)
        {
            put_pixel(sdl_data.current->img, x1, y1 + i, pixel);
            put_pixel(sdl_data.current->img, x1 - w, y1 + i, pixel);
        }
    }
    // redraws surface
    SDL_Surface *tmp = compress_frame(-1);
}

void circle(int x, int y, int r)
{
    // TODO
    // draws a circle with center coordinates of (x,y) and od radius r
    static const double PI = 3.1415926535;
    double i, angle, x1, y1;
    for (i = 0; i < 360; i += 0.1)
    {
        angle = i;
        x1 = r * cos(angle * PI / 180);
        y1 = r * sin(angle * PI / 180);
        //Uint32 p = SDL_MapRGBA(sdl_data.current->img->format, color->red * 255, color->green* 255, color->blue * 255, color->alpha*255);
        Uint32 p = SDL_MapRGB(sdl_data.current->img->format, 50, 50, 50);
        put_pixel(sdl_data.current->img, x + x1, y + y1, p);
    }
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
    sdl_data.nbframe = 1;
    sdl_data.curlayer = 0;
    sdl_data.curframe = 0;
    circle(10,10,5);
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

void main_sdl_import(char *filename)
{
    SDL_Surface *import = IMG_Load(filename);

    sdl_data.width = import->w;
    sdl_data.height = import->h;
    sdl_data.frames = init_frame(import->w, import->h);
    sdl_data.current = sdl_data.frames->next->layer->next;
    SDL_Surface *betterimport = SDL_ConvertSurface(import, 
            sdl_data.current->img->format, 0);
    SDL_FreeSurface(sdl_data.current->img);
    SDL_FreeSurface(import);
    sdl_data.current->img = betterimport;
    sdl_data.nblayer = 1;
    sdl_data.nbframe = 1;
    sdl_data.curlayer = 0;
    sdl_data.curframe = 0;


    // SDL_FreeSurface(screen_surface);
}

