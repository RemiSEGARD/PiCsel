#include "SDL.h"
#include "selection.h"
#include "../sdl/sdl_treatment.h"
#include "inputs.h"

// Selection

SDL_Rect select_pos;
SDL_Rect selecttmp_pos;
SDL_Rect clipboard_pos;

// id of timeout function
int id_to = 0;
int is_glow = 0;

void draw_glow(gpointer data)
{
    SDL_Surface *s = previs_select(select_pos.x, select_pos.y, select_pos.x + select_pos.w,
            select_pos.y + select_pos.h);
    is_glow = 1;
    redraw_surface((GtkDrawingArea *)data, s);
}

gboolean glow(gpointer data)
{
    if (!is_glow)
    {
        draw_glow(data);
    }
    else
    {
        SDL_Surface *s = compress_frame(-1, 1);
        redraw_surface((GtkDrawingArea *)data, s);    
        is_glow = 0;
    }
    return TRUE;
}

void blit_selects()
{
    for (int i = 0; i < select_pos.w; i++)
    {
        for (int j = 0; j < select_pos.h; j++)
        {
            if (select_pos.x + i >= 0 && select_pos.y + j>= 0
                    && select_pos.x + i < sdl_data.width && select_pos.y + j < sdl_data.width)
            {
                Uint32 pixel = get_pixel(sdl_data.selecttmp, 
                        selecttmp_pos.x + i, selecttmp_pos.y + j);
                put_pixel(sdl_data.select, select_pos.x + i, select_pos.y + j, pixel);
            }
        }
    }
}

void cp_surf(SDL_Surface *src, SDL_Surface *dst)
{
    for (int i = 0; i < src->w; i++)
    {
        for (int j = 0; j < src->h; j++)
        {
            Uint32 pixel = get_pixel(src, i, j);
            put_pixel(dst, i, j, pixel);
        }
    }
    
}

void deselect()
{
    if (select_pos.x != -1)
    {
        for (int i = select_pos.x; i < select_pos.x + select_pos.w; i++)
        {
            for (int j = select_pos.y; j < select_pos.y + select_pos.h; j++)
            {
                Uint32 pixel = get_pixel(sdl_data.select, i, j);
                if (pixel != 0)
                {
                    put_pixel(sdl_data.current->img, i, j, pixel);
                    put_pixel(sdl_data.select, i, j, 0);
                }
            }
        }
    }
    SDL_FillRect(sdl_data.previs, NULL, 0);
    SDL_FillRect(sdl_data.selecttmp, NULL, 0);
    if (id_to != 0)
    {
        g_source_remove(id_to);
        id_to = 0;
    }

}

void selection_press(int x, int y, int win_x, int win_y, GtkWidget *widget)
{
    deselect();

    if ((double)(sdl_data.width) / win_x > (double)(sdl_data.height) / win_y)
    {
        select_pos.x = x * sdl_data.width / (win_x - win_x % sdl_data.width);
        select_pos.y = y * sdl_data.width / (win_x - win_x % sdl_data.width);
    }
    else
    {
        select_pos.x = x * sdl_data.height / (win_y - win_y % sdl_data.height);
        select_pos.y = y * sdl_data.height / (win_y - win_y % sdl_data.height);
    }
    SDL_Surface *s = compress_frame(-1, 1);
    redraw_surface((GtkDrawingArea *)widget, s);
}

void selection_motion(int x, int y, int win_x, int win_y, GtkWidget *widget)
{
    SDL_Surface *s;
    if ((double)(sdl_data.width) / win_x > (double)(sdl_data.height) / win_y)
    {
        s = previs_select(
                select_pos.x,
                select_pos.y,
                x * sdl_data.width / (win_x - win_x % sdl_data.width),
                y * sdl_data.width / (win_x - win_x % sdl_data.width));
    }
    else
    {
        s = previs_select(
                select_pos.x,
                select_pos.y,
                x * sdl_data.height / (win_y - win_y % sdl_data.height),
                y * sdl_data.height / (win_y - win_y % sdl_data.height));
    }
    redraw_surface((GtkDrawingArea *)widget, s);
}

void selection_release(int x, int y, int win_x, int win_y, GtkWidget *darea)
{
    if (x1 == x && y1 == y)
    {
        select_pos.x = -1;
        select_pos.y = -1;
        select_pos.w = -1;
        select_pos.h = -1;
        return;
    }
    int tmpx;
    int tmpy;
    if ((double)(sdl_data.width) / win_x > (double)(sdl_data.height) / win_y)
    {
        tmpx = x * sdl_data.width / (win_x - win_x % sdl_data.width);
        tmpy = y * sdl_data.width / (win_x - win_x % sdl_data.width);
    }
    else
    {
        tmpx = x * sdl_data.height / (win_y - win_y % sdl_data.height);
        tmpy = y * sdl_data.height / (win_y - win_y % sdl_data.height);
    }
    if (select_pos.x > tmpx)
    {
        int tmp = tmpx;
        tmpx = select_pos.x;
        select_pos.x = tmp;
    }
    if (select_pos.y > tmpy) 
    {
        int tmp = tmpy;
        tmpy = select_pos.y;
        select_pos.y = tmp;
    }
    if (select_pos.x >= sdl_data.width || select_pos.y >= sdl_data.height)
    {
        select_pos.x = -1;
        select_pos.y = -1;
        select_pos.w = -1;
        select_pos.h = -1;
        return;
    }
    select_pos.x = CLAMP(select_pos.x, 0, sdl_data.width - 1);
    select_pos.y = CLAMP(select_pos.y, 0, sdl_data.height - 1);
    tmpx = CLAMP(tmpx, 0, sdl_data.width);
    tmpy = CLAMP(tmpy, 0, sdl_data.height);
    select_pos.w = tmpx - select_pos.x;
    select_pos.h = tmpy - select_pos.y;
    for (int i = select_pos.x; i < tmpx; i++)
    {
        for (int j = select_pos.y; j < tmpy; j++)
        {
            Uint32 pixel = get_pixel(sdl_data.current->img, i, j);
            put_pixel(sdl_data.select, i, j, pixel);
            put_pixel(sdl_data.selecttmp, i, j, pixel);
            put_pixel(sdl_data.current->img, i, j, 0);
        }
    }
    selecttmp_pos.x = select_pos.x;
    selecttmp_pos.y = select_pos.y;
    selecttmp_pos.w = select_pos.w;
    selecttmp_pos.h = select_pos.h;
    id_to = g_timeout_add(500, glow, darea);
}

void delete_selection()
{
    SDL_FillRect(sdl_data.select, NULL, 0);
    SDL_FillRect(sdl_data.selecttmp, NULL, 0);
    deselect();
}

void copy_selection()
{
    cp_surf(sdl_data.select, sdl_data.clipboard);
    clipboard_pos.x = select_pos.x;
    clipboard_pos.y = select_pos.y;
    clipboard_pos.w = select_pos.w;
    clipboard_pos.h = select_pos.h;
}

void cut_selection()
{
    cp_surf(sdl_data.select, sdl_data.clipboard);
    clipboard_pos.x = select_pos.x;
    clipboard_pos.y = select_pos.y;
    clipboard_pos.w = select_pos.w;
    clipboard_pos.h = select_pos.h;
    SDL_FillRect(sdl_data.select, NULL, 0);
    SDL_FillRect(sdl_data.selecttmp, NULL, 0);
    deselect();
}

void paste(gpointer data)
{
    deselect();
    cp_surf(sdl_data.clipboard, sdl_data.select);
    cp_surf(sdl_data.clipboard, sdl_data.selecttmp);
    select_pos.x = clipboard_pos.x;
    select_pos.y = clipboard_pos.y;
    select_pos.w = clipboard_pos.w;
    select_pos.h = clipboard_pos.h;
    selecttmp_pos.x = clipboard_pos.x;
    selecttmp_pos.y = clipboard_pos.y;
    selecttmp_pos.w = clipboard_pos.w;
    selecttmp_pos.h = clipboard_pos.h;
    if (id_to != 0)
        g_source_remove(id_to);
    id_to = g_timeout_add(500, glow, data);
    draw_glow(data);
}

void select_all(gpointer data)
{
    select_pos.x = 0;
    select_pos.y = 0;
    select_pos.w = sdl_data.width;
    select_pos.h = sdl_data.height;
    for (int i = select_pos.x; i < sdl_data.width; i++)
    {
        for (int j = select_pos.y; j < sdl_data.height; j++)
        {
            Uint32 pixel = get_pixel(sdl_data.current->img, i, j);
            put_pixel(sdl_data.select, i, j, pixel);
            put_pixel(sdl_data.selecttmp, i, j, pixel);
            put_pixel(sdl_data.current->img, i, j, 0);
        }
    }
    selecttmp_pos.x = select_pos.x;
    selecttmp_pos.y = select_pos.y;
    selecttmp_pos.w = select_pos.w;
    selecttmp_pos.h = select_pos.h;
    if (id_to != 0)
        g_source_remove(id_to);
    id_to = g_timeout_add(500, glow, data);
    draw_glow(data);
}

