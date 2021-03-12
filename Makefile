# Simple SDL mini Makefile

CC=gcc

CPPFLAGS= `pkg-config --cflags sdl` -MMD
CFLAGS= `pkg-config --cflags gtk+-3.0` -Wall -Wextra -std=c99 -O3 -g
#-fsanitize=address
LDFLAGS=
LDLIBS= `pkg-config --libs gtk+-3.0` `pkg-config --libs sdl` -lSDL_image -lm

all: PiCsel

main: main.o ui.o



clean:
	${RM} *.o
	${RM} *.d

# END
