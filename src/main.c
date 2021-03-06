#include "ui/ui.h"
#include <stdio.h>
#include <err.h>
#include <stdlib.h>

int main(int argc, char **argv)
{
    if (argc > 3 || argc < 2)
        errx(1, "Usage : ./main <x> <y> OR ./main <filename>");
    if (argc == 3)
    {
        int x = atoi(argv[1]);
        int y = atoi(argv[2]);
        if (x <= 0 || y <= 0)
            errx(1, "size cannot be smaller or equal to 0");

        main_ui(x, y, NULL);
    }
    if (argc == 2)
    {
        main_ui(0, 0, argv[1]);
    }

    return 0;
}
