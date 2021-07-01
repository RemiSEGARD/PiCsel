#include <stdio.h>
#include <err.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include "../sdl/img_frame.h"
#include "../sdl/img_layer.h"
#include "SDL.h"
#include "SDL_image.h"
#include "../sdl/sdl_treatment.h"

int read_int_line(int fd)
{
    char buf1[1];
    int r;
    int res = 0;
    while ((r = read(fd, buf1, 1)) != 0 && buf1[0] != '\n')
    {
        if (r == -1)
            errx(1, "could not read from file");
        res *= 10;
        res += buf1[0] - '0';
    }
    return res;
}

void read_img_line(int fd, SDL_Surface *s)
{
    char buf[16];
    int r;
    for (int i = 0; i < s->w; i++)
    {
        for (int j = 0; j < s->h; j++)
        {
            r = read(fd, buf, 16);
            if (r == -1)
                errx(1, "could not read from file");
            buf[3] = 0;
            buf[7] = 0;
            buf[11] = 0;
            buf[15] = 0;
            Uint32 pixel = SDL_MapRGBA(s->format, 
                    atoi(buf), atoi(buf + 4), atoi(buf + 8), atoi(buf + 12));
            put_pixel(s, j, i, pixel);
        }
    }
}

void write_img_line(FILE *f, SDL_Surface *s)
{
    for (int i = 0; i < s->w; i++)
    {
        for (int j = 0; j < s->h; j++)
        {
            Uint8 r, g, b, a;
            SDL_GetRGBA(get_pixel(s, j, i), s->format, &r, &g, &b, &a);
            fprintf(f, "%03u %03u %03u %03u", r, g, b, a);
            if (j != s->h - 1 || i != s->w - 1)
                fprintf(f, "|");
        }
    }
    fprintf(f, "\n");
}

void export_picsel(char *filename, SDLdata *sdl_data)
{
    FILE *f = fopen(filename, "w");
    fprintf(f, "%u\n%u\n%u\n%u\n", sdl_data->width, sdl_data->height,
            sdl_data->nbframe, sdl_data->nblayer);
    printf("%u %u \n", sdl_data->nbframe, sdl_data->nblayer);
    Frame *frame = sdl_data->frames->next;
    while (frame != NULL)
    {
        Layer *lay = frame->layer->next;
        while (lay != NULL)
        {
            write_img_line(f, lay->img);
            lay = lay->next;
        }
        frame = frame->next;
    }
    fclose(f);
}

Frame *import_picsel(char *filename)
{
    int fd = open(filename, O_RDONLY);
    int w = read_int_line(fd);
    int h = read_int_line(fd);
    int nb_frame = read_int_line(fd);
    int nb_layer = read_int_line(fd);
    
    Frame *frame = init_frame(w, h);

    for (int i = 0; i < nb_layer - 1; i++)
    {
        add_layer_to_all_frames(frame, w, h);
    }
    for (int i = 0; i < nb_frame - 1; i++)
    {
        add_frame(frame, w, h, nb_layer);
    }

    Frame *curframe = frame->next;
    for (int i = 0; i < nb_frame; i++)
    {
        Layer *curlayer = curframe->layer->next;
        for (int j = 0; j < nb_layer; j++)
        {
            read_img_line(fd, curlayer->img);
            curlayer = curlayer->next;
        }
        curframe = curframe->next;
    }
    close(fd);
    
    return frame;
}
