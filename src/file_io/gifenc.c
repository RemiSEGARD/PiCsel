#include "gifenc.h"
#include "fileio_picsel.h"
#include "gtk/gtk.h"
#include <SDL/SDL.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#ifdef _WIN32
#include <io.h>
#else
#include <unistd.h>
#endif

/* helper to write a little-endian 16-bit number portably */
#define write_num(fd, n) write((fd), (uint8_t []) {(n) & 0xFF, (n) >> 8}, 2)

static uint8_t vga[0x30] = {
    0x00, 0x00, 0x00,
    0xAA, 0x00, 0x00,
    0x00, 0xAA, 0x00,
    0xAA, 0x55, 0x00,
    0x00, 0x00, 0xAA,
    0xAA, 0x00, 0xAA,
    0x00, 0xAA, 0xAA,
    0xAA, 0xAA, 0xAA,
    0x55, 0x55, 0x55,
    0xFF, 0x55, 0x55,
    0x55, 0xFF, 0x55,
    0xFF, 0xFF, 0x55,
    0x55, 0x55, 0xFF,
    0xFF, 0x55, 0xFF,
    0x55, 0xFF, 0xFF,
    0xFF, 0xFF, 0xFF,
};

struct Node {
    uint16_t key;
    struct Node *children[];
};
typedef struct Node Node;

    static Node *
new_node(uint16_t key, int degree)
{
    Node *node = calloc(1, sizeof(*node) + degree * sizeof(Node *));
    if (node)
        node->key = key;
    return node;
}

    static Node *
new_trie(int degree, int *nkeys)
{
    Node *root = new_node(0, degree);
    /* Create nodes for single pixels. */
    for (*nkeys = 0; *nkeys < degree; (*nkeys)++)
        root->children[*nkeys] = new_node(*nkeys, degree);
    *nkeys += 2; /* skip clear code and stop code */
    return root;
}

    static void
del_trie(Node *root, int degree)
{
    if (!root)
        return;
    for (int i = 0; i < degree; i++)
        del_trie(root->children[i], degree);
    free(root);
}

#define write_and_store(s, dst, fd, src, n) \
    int w; \
    (void) w; \
    do { \
        w = write(fd, src, n); \
        if (s) { \
            memcpy(dst, src, n); \
            dst += n; \
        } \
    } while (0);

static void put_loop(ge_GIF *gif, uint16_t loop);

    ge_GIF *
ge_new_gif(
        const char *fname, uint16_t width, uint16_t height,
        uint8_t *palette, int depth, int loop
        )
{
    int w;
    (void) w;
    int i, r, g, b, v;
    int store_gct, custom_gct;
    ge_GIF *gif = calloc(1, sizeof(*gif) + 2*width*height);
    if (!gif)
        goto no_gif;
    gif->w = width; gif->h = height;
    gif->frame = (uint8_t *) &gif[1];
    gif->back = &gif->frame[width*height];
#ifdef _WIN32
    gif->fd = creat(fname, S_IWRITE);
#else
    gif->fd = creat(fname, 0666);
#endif
    if (gif->fd == -1)
        goto no_fd;
#ifdef _WIN32
    setmode(gif->fd, O_BINARY);
#endif
    w = write(gif->fd, "GIF89a", 6);
    w = write_num(gif->fd, width);
    w = write_num(gif->fd, height);
    store_gct = custom_gct = 0;
    if (palette) {
        if (depth < 0)
            store_gct = 1;
        else
            custom_gct = 1;
    }
    if (depth < 0)
        depth = -depth;
    gif->depth = depth > 1 ? depth : 2;
    w = write(gif->fd, (uint8_t []) {0xF0 | (depth-1), 0x00, 0x00}, 3);
    if (custom_gct) {
        w = write(gif->fd, palette, 3 << depth);
    } else if (depth <= 4) {
        write_and_store(store_gct, palette, gif->fd, vga, 3 << depth);
    } else {
        write_and_store(store_gct, palette, gif->fd, vga, sizeof(vga));
        i = 0x10;
        for (r = 0; r < 6; r++) {
            for (g = 0; g < 6; g++) {
                for (b = 0; b < 6; b++) {
                    write_and_store(store_gct, palette, gif->fd,
                            ((uint8_t []) {r*51, g*51, b*51}), 3
                            );
                    if (++i == 1 << depth)
                        goto done_gct;
                }
            }
        }
        for (i = 1; i <= 24; i++) {
            v = i * 0xFF / 25;
            write_and_store(store_gct, palette, gif->fd,
                    ((uint8_t []) {v, v, v}), 3
                    );
        }
    }


done_gct:
    if (loop >= 0 && loop <= 0xFFFF)
        put_loop(gif, (uint16_t) loop);
    return gif;
no_fd:
    free(gif);
no_gif:
    return NULL;
}

    static void
put_loop(ge_GIF *gif, uint16_t loop)
{
    int w;
    (void) w;
    w = write(gif->fd, (uint8_t []) {'!', 0xFF, 0x0B}, 3);
    w = write(gif->fd, "NETSCAPE2.0", 11);
    w = write(gif->fd, (uint8_t []) {0x03, 0x01}, 2);
    w = write_num(gif->fd, loop);
    w = write(gif->fd, "\0", 1);
}

/* Add packed key to buffer, updating offset and partial.
 *   gif->offset holds position to put next *bit*
 *   gif->partial holds bits to include in next byte */
    static void
put_key(ge_GIF *gif, uint16_t key, int key_size)
{
    int byte_offset, bit_offset, bits_to_write;
    int w;
    (void) w;
    byte_offset = gif->offset / 8;
    bit_offset = gif->offset % 8;
    gif->partial |= ((uint32_t) key) << bit_offset;
    bits_to_write = bit_offset + key_size;
    while (bits_to_write >= 8) {
        gif->buffer[byte_offset++] = gif->partial & 0xFF;
        if (byte_offset == 0xFF) {
            w = write(gif->fd, "\xFF", 1);
            w = write(gif->fd, gif->buffer, 0xFF);
            byte_offset = 0;
        }
        gif->partial >>= 8;
        bits_to_write -= 8;
    }
    gif->offset = (gif->offset + key_size) % (0xFF * 8);
}

    static void
end_key(ge_GIF *gif)
{
    int byte_offset;
    int w;
    (void) w;
    byte_offset = gif->offset / 8;
    if (gif->offset % 8)
        gif->buffer[byte_offset++] = gif->partial & 0xFF;
    if (byte_offset) {
        w = write(gif->fd, (uint8_t []) {byte_offset}, 1);
        w = write(gif->fd, gif->buffer, byte_offset);
    }
    w = write(gif->fd, "\0", 1);
    gif->offset = gif->partial = 0;
}

    static void
put_image(ge_GIF *gif, uint16_t w, uint16_t h, uint16_t x, uint16_t y)
{
    int nkeys, key_size, i, j;
    Node *node, *child, *root;
    int degree = 1 << gif->depth;
    int wr;
    (void) wr;
    wr = write(gif->fd, ",", 1);
    wr = write_num(gif->fd, x);
    wr = write_num(gif->fd, y);
    wr = write_num(gif->fd, w);
    wr = write_num(gif->fd, h);
    wr = write(gif->fd, (uint8_t []) {0x00, gif->depth}, 2);
    root = node = new_trie(degree, &nkeys);
    key_size = gif->depth + 1;
    put_key(gif, degree, key_size); /* clear code */
    for (i = y; i < y+h; i++) {
        for (j = x; j < x+w; j++) {
            uint8_t pixel = gif->frame[i*gif->w+j] & (degree - 1);
            child = node->children[pixel];
            if (child) {
                node = child;
            } else {
                put_key(gif, node->key, key_size);
                if (nkeys < 0x1000) {
                    if (nkeys == (1 << key_size))
                        key_size++;
                    node->children[pixel] = new_node(nkeys++, degree);
                } else {
                    put_key(gif, degree, key_size); /* clear code */
                    del_trie(root, degree);
                    root = node = new_trie(degree, &nkeys);
                    key_size = gif->depth + 1;
                }
                node = root->children[pixel];
            }
        }
    }
    put_key(gif, node->key, key_size);
    put_key(gif, degree + 1, key_size); /* stop code */
    end_key(gif);
    del_trie(root, degree);
}

    static int
get_bbox(ge_GIF *gif, uint16_t *w, uint16_t *h, uint16_t *x, uint16_t *y)
{
    int i, j, k;
    int left, right, top, bottom;
    left = gif->w; right = 0;
    top = gif->h; bottom = 0;
    k = 0;
    for (i = 0; i < gif->h; i++) {
        for (j = 0; j < gif->w; j++, k++) {
            if (gif->frame[k] != gif->back[k]) {
                if (j < left)   left    = j;
                if (j > right)  right   = j;
                if (i < top)    top     = i;
                if (i > bottom) bottom  = i;
            }
        }
    }
    if (left != gif->w && top != gif->h) {
        *x = left; *y = top;
        *w = right - left + 1;
        *h = bottom - top + 1;
        return 1;
    } else {
        return 0;
    }
}

    static void
set_delay(ge_GIF *gif, uint16_t d)
{
    int w;
    (void) w;
    w = write(gif->fd, (uint8_t []) {'!', 0xF9, 0x04, 0x04}, 4);
    w = write_num(gif->fd, d);
    w = write(gif->fd, "\0\0", 2);
}

    void
ge_add_frame(ge_GIF *gif, uint16_t delay)
{
    uint16_t w, h, x, y;
    uint8_t *tmp;

    if (delay)
        set_delay(gif, delay);
    if (gif->nframes == 0) {
        w = gif->w;
        h = gif->h;
        x = y = 0;
    } else if (!get_bbox(gif, &w, &h, &x, &y)) {
        /* image's not changed; save one pixel just to add delay */
        w = h = 1;
        x = y = 0;
    }
    put_image(gif, w, h, x, y);
    gif->nframes++;
    tmp = gif->back;
    gif->back = gif->frame;
    gif->frame = tmp;
}

    void
ge_close_gif(ge_GIF* gif)
{
    int w = write(gif->fd, ";", 1);
    (void) w;
    close(gif->fd);
    free(gif);
}

Uint8 *create_palette()
{
    Uint8* p = malloc(sizeof(Uint8) * 256 * 3);
    memcpy(p, vga, sizeof(vga));
    Uint8 *tmp = p + 0x30;
    for (int r = 0; r < 6; r++)
    {
        for (int g = 0; g < 6; g++)
        {
            for (int b = 0; b < 6; b++)
            {
                *tmp = r * 51;
                tmp++;
                *tmp = g * 51;
                tmp++;
                *tmp = b * 51;
                tmp++;
            }
        }
    }
    for (int i = 0; i < 24; i++)
    {
        for (int j = 0; j < 3; j++)
        {
            *tmp = i * 0xff / 25;
            tmp++;
        }
    }
    
    return p;
}

Uint8 get_nearest(Uint8 *p, Uint8 r, Uint8 g, Uint8 b)
{
    unsigned int near_per = 0;
    Uint8 res = 0;
    for (int i = 0; i < 256 * 3; i += 3)
    {
        unsigned int per_r = *(p + i);
        if (*(p + i) > r )
            per_r = r * 100 / (*(p + i) + 1);
        else
            per_r = *(p + i) * 100 / (r + 1);

        unsigned int per_g = *(p + i + 1);
        if (*(p + i + 1) > g )
            per_g = g * 100 / (*(p + i + 1) + 1);
        else
            per_g = *(p + i + 1) * 100 / (g + 1);

        unsigned int per_b = *(p + i + 2);
        if (*(p + i + 2) > b )
            per_b = b * 100 / (*(p + i + 2) + 1);
        else
            per_b = *(p + i + 2) * 100 / (b + 1);

        unsigned int per = (per_r + per_g + per_b) / 3;
        if (per > near_per)
        {
            near_per = per;
            res = i / 3;
        }
    }
    return res;
}

void export_gif(char *filename, SDLdata *sdl_data)
{
    ge_GIF *gif = ge_new_gif(
            filename,
            sdl_data->width,sdl_data->height,
            NULL,
            8,
            0
            );
    // draw
    
    Uint8 *p = create_palette();

    Frame *list = sdl_data->frames->next;
    while (list != NULL)
    {
        SDL_Surface *surface = compress_frame(list->index,0);
        for (int i = 0; i < sdl_data->width; i++)
        {
            for (int j = 0; j < sdl_data->height; j++)
            {
                Uint32 pixel = get_pixel(surface,i,j);
                Uint8 r, g, b;
                SDL_GetRGB(pixel, surface->format, &r, &g, &b);
                Uint8 res = get_nearest(p, r, g, b);
                gif->frame[j * sdl_data->height + i] = res;
            }
        }
        ge_add_frame(gif, list->duration/10);
        list = list->next;
    }

    ge_close_gif(gif);
}
