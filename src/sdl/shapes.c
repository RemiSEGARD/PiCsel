#include "sdl_treatment.h"
#include <math.h>

void line(int x1, int y1, int x2, int y2,int win_x,int win_y, GdkRGBA* color)
{
    // Draws the line from (x1,y1) to (x2,y2) onthe SDL_Surface
    if ((double) sdl_data.width / win_x > (double) sdl_data.height / win_y)
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
    // Convert GdkRGBA color to a value usable by SDL
    Uint32 pixel = SDL_MapRGBA(sdl_data.current->img->format, color->red * 255, color->green* 255, color->blue * 255, color->alpha*255);

    double x, y, dx, dy, step;
    dx = (float)(x2 - x1);
    dy = (float)(y2 - y1);
    if (fabs(dx) > fabs(dy))
        step = fabs(dx);
    else
        step = fabs(dy);
    dx = dx / step;
    dy = dy / step;
    x = x1;
    y = y1;
    int i = 0;
    while (i <= step)
    {
        put_pixel(sdl_data.current->img, (int)x, (int)y, pixel);
        x += dx;
        y += dy;
        i = i + 1;
    }
}


void rectangle(int x1, int y1, int x2, int y2, int win_x, int win_y, GdkRGBA* color)
{
    // Draws a rectangle from (x1,y1) to (x2,y2) on the SDL_Surface
    // Calculate the new coordinates
    if ((double) sdl_data.width / win_x > (double) sdl_data.height / win_y)
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
    // Swap variables if necessary
    if (x2 < x1)
    {
        int tmp = x1;
        x1 = x2;
        x2 = tmp;
    }
    if (y2 < y1)
    {
        int tmp = y1;
        y1 = y2;
        y2 = tmp;
    }

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

void circle(int x1, int y1, int x2,int y2,int win_x,int win_y, GdkRGBA* color)
{
    // draws a circle "between" the coordonates (x1,y1) and (x2,y2)

    // Calculate the new coordinates
    if ((double) sdl_data.width / win_x > (double) sdl_data.height / win_y)
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
    // Convert GdkRGBA color to a value usable by SDL
    Uint32 pixel = SDL_MapRGBA(sdl_data.current->img->format, color->red * 255, color->green* 255, color->blue * 255, color->alpha*255);

    // Calculate the radius and center of the circle
    
    if (x1 > x2 && y1 > y2)
    {
        int tmp = x1;
        x1 = x2;
        x2 = tmp;
        tmp = y1;
        y1 = y2;
        y2 = tmp;
    }
    if(x1>x2 && y1<y2)
    {
        int tmp = x1;
        x1 = x2;
        x2 = tmp;
        tmp = y1;
        y1 = y2;
        y2 = tmp;
    }

    int center_x = (x1+x2)/2;
    int center_y = (y1+y2)/2;
    // highest x coordinate
    int r = x2 - center_x;

    // Bresenham
    int x = 0;
    int y = r;
    int d = 3 - 2 * r;

    // Draws to the 8 point of the circle
    if (x >= 0 || y >= 0 || y <= sdl_data.height || x <= sdl_data.width)
        put_pixel(sdl_data.current->img, center_x + x, center_y + y, pixel);
    put_pixel(sdl_data.current->img, center_x + y, center_y + x, pixel);
    put_pixel(sdl_data.current->img, center_x - y, center_y + x, pixel);
    put_pixel(sdl_data.current->img, center_x - x, center_y + y, pixel);
    put_pixel(sdl_data.current->img, center_x - x, center_y - y, pixel);
    put_pixel(sdl_data.current->img, center_x + y, center_y - x, pixel);
    put_pixel(sdl_data.current->img, center_x + x, center_y - y, pixel);
    put_pixel(sdl_data.current->img, center_x - y, center_y - x, pixel);


    while(y>=x)
    {
        x++;
        if (d>0)
        {
            y--;
            d = d+4*(x-y)+10;
        }
        else
            d = d + 4 * x + 6;

        put_pixel(sdl_data.current->img, center_x + x, center_y + y, pixel);
        put_pixel(sdl_data.current->img, center_x + y, center_y + x, pixel);
        put_pixel(sdl_data.current->img, center_x - y, center_y + x, pixel);
        put_pixel(sdl_data.current->img, center_x - x, center_y + y, pixel);
        put_pixel(sdl_data.current->img, center_x - x, center_y - y, pixel);
        put_pixel(sdl_data.current->img, center_x + y, center_y - x, pixel);
        put_pixel(sdl_data.current->img, center_x + x, center_y - y, pixel);
        put_pixel(sdl_data.current->img, center_x - y, center_y - x, pixel);
    }
}

static void fill_sdl(int x, int y, GdkRGBA* color, Uint32 match)
{
    // Fills on the SDL_Surface
    // match_r, match_g, match_b, match_a represent the rgba values needed to match the color
    if (x < 0 || y < 0 || x >= sdl_data.width || y >= sdl_data.height)
        return;
    Uint8 r,g,b,a;
    Uint32 pixel = get_pixel(sdl_data.current->img, x, y);
    SDL_GetRGBA(pixel, sdl_data.current->img->format, &r, &g, &b, &a);

    int cr = color->red*255;
    int cg = color->green*255;
    int cb = color->blue*255;
    int ca = color->alpha*255;

    if (r == cr && g == cg && b == cb && a == ca)
    {
        return;
    }
    // condition needs to be changed to detect the proper pixel now it only detects white pixels
    if (match == pixel)
    {
        // converts the color from GdkRGBA to a value from 0 to 255
        Uint32 p = SDL_MapRGBA(sdl_data.current->img->format, color->red * 255, color->green* 255, color->blue * 255, color->alpha*255);
        put_pixel(sdl_data.current->img, x, y, p);
        // launches all the recursive function to draw the pixels around current pixel
        fill_sdl(x+1,y,color,match);
        fill_sdl(x-1,y,color,match);
        fill_sdl(x,y+1,color,match);
        fill_sdl(x,y-1,color,match);
    }
}

void fill(int x, int y, int win_x, int win_y, GdkRGBA* color)
{
    // function called by drawing.c
    // prepares everything for the actual fill function
    if (sdl_data.current->shown == 0 ) return;
    // calculate the new coordinates
    if ((double) sdl_data.width / win_x > (double) sdl_data.height / win_y)
    {
        x = x * sdl_data.width / (win_x - win_x % sdl_data.width);
        y = y * sdl_data.width / (win_x - win_x % sdl_data.width);
    }
    else
    {
        x = x * sdl_data.height / (win_y - win_y % sdl_data.height);
        y = y * sdl_data.height / (win_y - win_y % sdl_data.height);
    }
    // gets the color that we want to replace by GdkRGBA* color
    Uint32 pixel = get_pixel(sdl_data.current->img, x, y);
    /*Uint8 r = color->red * 255;
    Uint8 g = color->green * 255;
    Uint8 b = color->blue * 255;
    Uint8 a = color->alpha * 255;
    Uint32 color_to_pixel = SDL_MapRGBA(sdl_current->img->format,r,g,b,a);*/
    fill_sdl(x,y,color,pixel);
}

GdkRGBA* eyedropper(int x, int y, int win_x, int win_y)
{
    if ((double) sdl_data.width / win_x > (double) sdl_data.height / win_y)
    {
        x = x * sdl_data.width / (win_x - win_x % sdl_data.width);
        y = y * sdl_data.width / (win_x - win_x % sdl_data.width);
    }
    else
    {
        x = x * sdl_data.height / (win_y - win_y % sdl_data.height);
        y = y * sdl_data.height / (win_y - win_y % sdl_data.height);
    }
    if (x < 0 || y < 0 || y >= sdl_data.height || x >= sdl_data.width)
        return NULL;

    Uint32 p = get_pixel(sdl_data.current->img,x,y);
    Uint8 r,g,b,a;
    SDL_GetRGBA(p, sdl_data.current->img->format, &r, &g, &b, &a);
    GdkRGBA* color = malloc(sizeof(GdkRGBA));
    double red = (double) r;
    double green = (double) g;
    double blue = (double) b;
    double alpha = (double) a;
    color->red = red / 255;
    color->green = green / 255;
    color->blue = blue / 255;
    color->alpha = alpha / 255;
    return color;
}
