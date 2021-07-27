# Makefile for PiCsel

CC=gcc

CPPFLAGS= `pkg-config --cflags sdl` -MMD
CFLAGS= `pkg-config --cflags gtk+-3.0` -Wall -Wextra -std=c99 -O3 -g -fsanitize=address
LDLIBS= `pkg-config --libs gtk+-3.0` `pkg-config --libs sdl` -lSDL_image -lpng -lm -fsanitize=address

main: main.o ui/ui.o ui/navigation.o ui/drawing.o \
	sdl/sdl_treatment.o sdl/img_frame.o sdl/img_layer.o sdl/shapes.o \
	file_io/fileio_picsel.o file_io/savesurf.o file_io/gifdec.o file_io/gifenc.o

clean:
	${RM} -r */*.o
	${RM} -r */*.d
	${RM} -r *.o
	${RM} -r *.d
	${RM} -r main 

# END
