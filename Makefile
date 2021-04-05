# Simple SDL mini Makefile

CC=gcc

CPPFLAGS= `pkg-config --cflags sdl` -MMD
CFLAGS= `pkg-config --cflags gtk+-3.0` -Wall -Wextra -std=c99 -O3 -g -fsanitize=address
LDLIBS= `pkg-config --libs gtk+-3.0` `pkg-config --libs sdl` -lSDL_image -lm -fsanitize=address

main: main.o ui.o drawing.o sdl_treatment.o img_frame.o img_layer.o display_sdl.o

clean:
	${RM} *.o
	${RM} *.d
	${RM} main 

# END
