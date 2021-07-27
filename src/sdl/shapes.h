#ifndef SHAPES_H
#define SHAPES_H

void line(int x1, int y1, int x2, int y2,int win_x,int win_y, GdkRGBA* color);

void rectangle(int x1, int y1, int x2, int y2,int win_x,int win_y, GdkRGBA* color);

void circle(int x1, int y1, int x2, int y2,int win_x,int win_y, GdkRGBA* color);

void fill(int x, int y, int win_x, int win_y, GdkRGBA* color);

GdkRGBA* eyedropper(int x, int y, int win_x, int win_y);

#endif
