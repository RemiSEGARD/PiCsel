#ifndef DISPLAY_SDL_H
#define DISPLAY_SDL_H

void init_sdl();

SDL_Surface* load_image(char *path);

SDL_Surface* display_image(SDL_Surface *img);

void wait_for_keypressed();

void update_surface(SDL_Surface* screen, SDL_Surface* image);

#endif
