#include <gtk/gtk.h>
#include <stdio.h>
#include "ui.h"
#include "SDL.h"
#include "drawing.h"
#include "../sdl/shapes.h"
#include "../sdl/sdl_treatment.h"
#include "../file_io/fileio_picsel.h"
#include <time.h>
#include <unistd.h>

/* Surface to store current scribbles */
static cairo_surface_t *surface = NULL;

// State Variables
Tools tool = DRAW;
gdouble x1,y1;

void deselect();

GtkColorChooser *colorselect;

void draw_background()
{
    cairo_t *cr;

    cr = cairo_create (surface);

    cairo_set_source_rgb (cr, 0.94901, 0.94509, 0.94117);
    cairo_paint (cr);

    cairo_destroy (cr);

}

void set_tool(Tools selected_tool)
{
    if (selected_tool != SELECT)
    {
        deselect();
        gtk_widget_set_sensitive((GtkWidget *)colorselect, TRUE);
    }
    else
    {
        gtk_widget_set_sensitive((GtkWidget *)colorselect, FALSE);
    }
    tool = selected_tool;
}

/* Create a new surface of the appropriate size to store our scribbles */
static gboolean configure_event_cb (GtkWidget *widget, 
        GdkEventConfigure *event, gpointer data)
{
    (void) event;
    (void) data;
    if (surface)
        cairo_surface_destroy (surface);

    surface = gdk_window_create_similar_surface (gtk_widget_get_window (widget),
            CAIRO_CONTENT_COLOR,
            gtk_widget_get_allocated_width (widget),
            gtk_widget_get_allocated_height (widget));
    
    /* Initialize the surface to white */
    draw_background();

    return TRUE;
}

/* Redraw the screen from the surface. Note that the ::draw
 * signal receives a ready-to-be-used cairo_t that is already
 * clipped to only draw the exposed areas of the widget
 */
static gboolean draw_cb(GtkWidget *widget, cairo_t *cr, gpointer data)
{
    (void) widget;
    (void) data;
    cairo_set_source_surface (cr, surface, 0, 0);
    cairo_paint (cr);

    return FALSE;
}

/* Draw a rectangle on the surface at the given position */
static void draw_brush (GtkWidget *widget, gdouble x, gdouble y, GdkRGBA* color)
{
    (void) widget;
    cairo_t *cr;
    
    /* Paint to the surface, where we store our state */
    cr = cairo_create (surface);
    GdkRectangle rect = calculate_coord(x, y, 
            gtk_widget_get_allocated_width(widget),
            gtk_widget_get_allocated_height(widget), color);
    
    cairo_set_source_rgba(cr, color->red, color->green, color->blue, color->alpha);
    
    //fill(x,y,gtk_widget_get_allocated_width(widget),gtk_widget_get_allocated_height(widget),color);
    
    cairo_rectangle (cr, rect.x, rect.y, rect.width, rect.height);
    cairo_fill (cr);
    
    cairo_destroy (cr);
    
    /* Now invalidate the affected region of the drawing area.
     * Invalidated regions of a widget are redrawn by Gtk
     * In the current case, it redraws the area of the new rectangle
    */
    gtk_widget_queue_draw_area (widget, rect.x, rect.y, 
            rect.width, rect.height);
}

/* Draw a rectangle on the surface at the given position of an image */
static void draw_pixel (GtkWidget *widget, int x, int y, SDL_Surface *s)
{
    (void) widget;
    cairo_t *cr;
    
    /* Paint to the surface, where we store our state */
    cr = cairo_create (surface);
    
    GdkRectangle rect; 
    int w = gtk_widget_get_allocated_width(widget);
    int h = gtk_widget_get_allocated_height(widget);
    int pixel_size;
    if ((double)(s->w) / w > (double)(s->h) / h)
        pixel_size = w / s->w;
    else
        pixel_size = h / s->h;
    rect.x = x * pixel_size;
    rect.y = y * pixel_size;
    rect.width = pixel_size;
    rect.height = pixel_size;
    

    cairo_rectangle (cr, rect.x, rect.y, rect.width, rect.height);
    
    Uint8 r, g, b;
    SDL_GetRGB(get_pixel(s, x, y), s->format, &r, &g, &b);
    cairo_set_source_rgb(cr,
            (double) r / 255, (double) g / 255, (double) b / 255);
    cairo_fill (cr);
    cairo_destroy (cr);
    cairo_set_source_rgb(cr, 0, 0, 0);
    
    /* Now invalidate the affected region of the drawing area.
     * Invalidated regions of a widget are redrawn by Gtk
     * In the current case, it redraws the area of the new rectangle
    */
    gtk_widget_queue_draw_area (widget, rect.x, rect.y, 
            rect.width, rect.height);
}



void redraw_surface(GtkDrawingArea *drawing_area, SDL_Surface *surf)
{
    cairo_t *cr;
    
    /* Paint to the surface, where we store our state */
    cr = cairo_create (surface);
    
    GdkRectangle rect; 
    int w = gtk_widget_get_allocated_width((GtkWidget *)drawing_area);
    int h = gtk_widget_get_allocated_height((GtkWidget *)drawing_area);
    int pixel_size;
    if ((double)(surf->w) / w > (double)(surf->h) / h)
        pixel_size = w / surf->w;
    else
        pixel_size = h / surf->h;
    rect.x = surf->w * pixel_size;
    rect.y = 0;
    rect.width = 3;
    rect.height = surf->h * pixel_size + 3;
    
    cairo_rectangle (cr, rect.x, rect.y, rect.width, rect.height);
    
    cairo_set_source_rgb(cr, 0, 0, 0);
    cairo_fill (cr);
    cairo_destroy (cr);
    gtk_widget_queue_draw_area ((GtkWidget *)drawing_area, rect.x, rect.y, 
            rect.width, rect.height);
    
    /* Paint to the surface, where we store our state */
    cr = cairo_create (surface);
    
    rect.x = 0;
    rect.y = surf->h * pixel_size;
    rect.width = surf->w * pixel_size;
    rect.height = 3;
    
    cairo_rectangle (cr, rect.x, rect.y, rect.width, rect.height);
    
    cairo_set_source_rgb(cr, 0, 0, 0);
    cairo_fill (cr);
    cairo_destroy (cr);
    
    gtk_widget_queue_draw_area((GtkWidget *)drawing_area, rect.x, rect.y, 
            rect.width, rect.height);
    for (int i = 0; i < surf->w; i++)
    {
        for (int j = 0; j < surf->h; j++)
        {
            draw_pixel((GtkWidget *)drawing_area, i , j, surf);
        }
    }
}

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

int is_ctrl = 0;

void move_select(GtkWidget *widget, GdkEventKey *event, gpointer user_data)
{
    (void) user_data;
    (void) widget;
    int moved = 0;
    if (event->keyval == GDK_KEY_Up)
    {
        moved = 1;
        select_pos.y -= 1;
    }
    else if (event->keyval == GDK_KEY_Down)
    {
        moved = 1;
        select_pos.y += 1;
    }
    else if (event->keyval == GDK_KEY_Left)
    {
        moved = 1;
        select_pos.x -= 1;
    }
    else if (event->keyval == GDK_KEY_Right)
    {
        moved = 1;
        select_pos.x += 1;
    }
    else if (event->keyval == GDK_KEY_Control_L || event->keyval == GDK_KEY_Control_R)
    {
        is_ctrl = 1;
    }
    else if (event->keyval == GDK_KEY_Delete)
    {
        SDL_FillRect(sdl_data.select, NULL, 0);
        SDL_FillRect(sdl_data.selecttmp, NULL, 0);
        deselect();
        SDL_Surface *s = compress_frame(-1, 1);
        redraw_surface((GtkDrawingArea *)user_data, s);
    }
    else if (is_ctrl)
    {
        if (event->keyval == GDK_KEY_c)
        {
            cp_surf(sdl_data.select, sdl_data.clipboard);
            clipboard_pos.x = select_pos.x;
            clipboard_pos.y = select_pos.y;
            clipboard_pos.w = select_pos.w;
            clipboard_pos.h = select_pos.h;
        }
        else if (event->keyval == GDK_KEY_x)
        {
            cp_surf(sdl_data.select, sdl_data.clipboard);
            clipboard_pos.x = select_pos.x;
            clipboard_pos.y = select_pos.y;
            clipboard_pos.w = select_pos.w;
            clipboard_pos.h = select_pos.h;
            SDL_FillRect(sdl_data.select, NULL, 0);
            SDL_FillRect(sdl_data.selecttmp, NULL, 0);
            deselect();
            SDL_Surface *s = compress_frame(-1, 1);
            redraw_surface((GtkDrawingArea *)user_data, s);
        }
        else if (event->keyval == GDK_KEY_a)
        {
            set_tool(SELECT);
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
            id_to = g_timeout_add(500, glow, user_data);
            draw_glow(user_data);
        }
        else if (event->keyval == GDK_KEY_v)
        {
            deselect();
            cp_surf(sdl_data.clipboard, sdl_data.select);
            cp_surf(sdl_data.clipboard, sdl_data.selecttmp);
            set_tool(SELECT);
            select_pos.x = clipboard_pos.x;
            select_pos.y = clipboard_pos.y;
            select_pos.w = clipboard_pos.w;
            select_pos.h = clipboard_pos.h;
            selecttmp_pos.x = clipboard_pos.x;
            selecttmp_pos.y = clipboard_pos.y;
            selecttmp_pos.w = clipboard_pos.w;
            selecttmp_pos.h = clipboard_pos.h;
            id_to = g_timeout_add(500, glow, user_data);
            draw_glow(user_data);
        }
    }
    if (moved)
    {
        SDL_FillRect(sdl_data.select, NULL, 0);
        blit_selects();
        SDL_Surface *s = compress_frame(-1, 1);
        redraw_surface((GtkDrawingArea *)user_data, s);
        if (id_to != 0)
        {
            g_source_remove(id_to);
            id_to = 0;
        }
        id_to = g_timeout_add(500, glow, user_data);
        draw_glow(user_data);
    }

}

void get_key_release(GtkWidget *widget, GdkEventKey *event, gpointer user_data)
{
    (void) widget;
    (void) user_data;
    if (event->keyval == GDK_KEY_Control_L || event->keyval == GDK_KEY_Control_R)
        is_ctrl = 0;
}


/* Handle the different types of button pressed */
static gboolean button_press_event_cb (GtkWidget *widget,
        GdkEventButton *event, gpointer data)
{
    if (surface == NULL)
        return FALSE;

    if (event->button == GDK_BUTTON_PRIMARY)
    {
        GdkRGBA* color = malloc(sizeof(GdkRGBA));
        gtk_color_chooser_get_rgba(data,color);
        switch(tool)
        {
            case DRAW:
                x1 = event->x;
                y1 = event->y;
                draw_brush (widget, event->x, event->y, color);
                break;
            case ERASER:
                x1 = event->x;
                y1 = event->y;
                color->red = 0;
                color->blue = 0;
                color->green = 0;
                color->alpha = 0;
                draw_brush (widget, event->x, event->y, color);
                break;
            case FILL:
                x1 = event->x;
                y1 = event->y;
                fill(event->x, event->y,
                        gtk_widget_get_allocated_width(widget),
                        gtk_widget_get_allocated_height(widget), color);
                SDL_Surface *s = compress_frame(-1, 1);
                redraw_surface((GtkDrawingArea *)widget, s);
                break;
            case SELECT:
                selection_press(event->x, event->y,
                        gtk_widget_get_allocated_width(widget),
                        gtk_widget_get_allocated_height(widget), widget);
                break;
            default:
                x1 = event->x;
                y1 = event->y;
                break;
        }
        free(color);
    }
    else if (event->button == GDK_BUTTON_SECONDARY)
    {
        GdkRGBA *p = eyedropper(event->x,event->y,gtk_widget_get_allocated_width(widget),gtk_widget_get_allocated_height(widget));
        if (p == NULL)
            return TRUE;
        gtk_color_chooser_set_rgba(data,p);
        gtk_widget_queue_draw (widget);
    }

    return TRUE;
}

/* Handle the held mouse button 1 event */
static gboolean motion_notify_event_cb (GtkWidget *widget,
        GdkEventMotion *event, gpointer data)
{
    (void) data;
    if (surface == NULL)
        return FALSE;
    int h = gtk_widget_get_allocated_height(widget);
    int w = gtk_widget_get_allocated_width(widget);

    GdkRGBA* color = malloc(sizeof(GdkRGBA));
    gtk_color_chooser_get_rgba(data,color);
    if (event->state & GDK_BUTTON1_MASK)
    {
        switch(tool)
        {
            case DRAW:
                line(x1 ,y1 , event->x, event->y, w, h,color);
                x1 = event->x;
                y1 = event->y;
                SDL_Surface *s = compress_frame(-1, 1);
                redraw_surface((GtkDrawingArea *)widget, s);
                break;
            case ERASER:
                color->red = 0;
                color->blue = 0;
                color->green = 0;
                color->alpha = 0;
                line(x1 ,y1 , event->x, event->y, w, h,color);
                x1 = event->x;
                y1 = event->y;
                s = compress_frame(-1, 1);
                redraw_surface((GtkDrawingArea *)widget, s);
                break;
            case RECTANGLE:
                s = previsualisation(rectangle, x1, y1, event->x, event->y, w, h, color);
                redraw_surface((GtkDrawingArea *)widget, s);
                break;
            case CIRCLE:
                s = previsualisation(circle, x1, y1, event->x, event->y, w, h, color);
                redraw_surface((GtkDrawingArea *)widget, s);
                break;
            case LINE:
                s = previsualisation(line, x1, y1, event->x, event->y, w, h, color);
                redraw_surface((GtkDrawingArea *)widget, s);
                break;
            case SELECT:
                selection_motion(event->x, event->y,
                        gtk_widget_get_allocated_width(widget),
                        gtk_widget_get_allocated_height(widget), widget);

                break;
            default:
                break;
        }
    }
    free(color);
    return TRUE;
}


static gboolean button_release_event_cb (GtkWidget *widget,
        GdkEventMotion *event, gpointer data)
{
    int h = gtk_widget_get_allocated_height(widget);
    int w = gtk_widget_get_allocated_width(widget);
    GdkRGBA* color = malloc(sizeof(GdkRGBA));
    gtk_color_chooser_get_rgba(data,color);

    if (event->state & GDK_BUTTON1_MASK)
    {
        switch(tool)
        {
            case LINE:
                line(x1 ,y1 , event->x, event->y, w, h,color);
                SDL_Surface *s = compress_frame(-1, 1);
                redraw_surface((GtkDrawingArea *)widget, s);
                break;
            case RECTANGLE:
                rectangle(x1, y1, event->x, event->y, w, h, color);
                s = compress_frame(-1, 1);
                redraw_surface((GtkDrawingArea *)widget, s);
                break;
            case CIRCLE :
                circle(x1,y1,event->x,event->y,w,h,color);
                s = compress_frame(-1, 1);
                redraw_surface((GtkDrawingArea *)widget, s);
                break;
            case SELECT:
                selection_release(event->x, event->y,
                        gtk_widget_get_allocated_width(widget),
                        gtk_widget_get_allocated_height(widget), widget);
                break;
            default:
                break;
        }
    }

    free(color);
    return TRUE;
}
// Setups the events for the drawing area
void setup_drawing(GtkDrawingArea *drawing_area, GtkColorChooser *color_select, GtkWindow *window)
{
    //      Signals for drawing
    //      Drawing areas do not handle clicks, this add the events
    gtk_widget_add_events((GtkWidget *) drawing_area, GDK_BUTTON_PRESS_MASK
            | GDK_POINTER_MOTION_MASK | GDK_BUTTON_RELEASE_MASK);
    //      Handle the backing surface
    g_signal_connect (drawing_area, "draw",
            G_CALLBACK (draw_cb), NULL);
    g_signal_connect (drawing_area,"configure-event",
            G_CALLBACK (configure_event_cb), NULL);
    //      Clicking events
    g_signal_connect (drawing_area, "motion-notify-event",
           G_CALLBACK (motion_notify_event_cb), color_select);
    g_signal_connect (drawing_area, "button-press-event",
            G_CALLBACK (button_press_event_cb), color_select);
    g_signal_connect (drawing_area, "button-release-event",
            G_CALLBACK (button_release_event_cb), color_select);
    g_signal_connect(window, "key_press_event", G_CALLBACK(move_select), drawing_area);
    g_signal_connect(window, "key_release_event", G_CALLBACK(get_key_release), NULL);
    colorselect = color_select;
    return;
}
