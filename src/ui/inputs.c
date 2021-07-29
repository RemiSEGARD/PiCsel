#include <gtk/gtk.h>
#include <stdio.h>
#include "ui.h"
#include "SDL.h"
#include "inputs.h"
#include "drawingarea.h"
#include "selection.h"
#include "../sdl/shapes.h"
#include "../sdl/sdl_treatment.h"
#include "../file_io/fileio_picsel.h"
#include <time.h>
#include <unistd.h>

/* Surface to store current scribbles */
cairo_surface_t *surface = NULL;

// State Variables
Tools tool = DRAW;
gdouble x1,y1;

GtkColorChooser *colorselect;


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

int is_ctrl = 0;

void keyboard_inputs(GtkWidget *widget, GdkEventKey *event, gpointer user_data)
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
        delete_selection();
        SDL_Surface *s = compress_frame(-1, 1);
        redraw_surface((GtkDrawingArea *)user_data, s);
    }
    else if (is_ctrl)
    {
        if (event->keyval == GDK_KEY_c)
        {
            copy_selection();
        }
        else if (event->keyval == GDK_KEY_x)
        {
            cut_selection();
            SDL_Surface *s = compress_frame(-1, 1);
            redraw_surface((GtkDrawingArea *)user_data, s);
        }
        else if (event->keyval == GDK_KEY_a)
        {
            select_all(user_data);
            set_tool(SELECT);
        }
        else if (event->keyval == GDK_KEY_v)
        {
            paste(user_data);
            set_tool(SELECT);
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
                fill(event->x, event->y, win_x, win_y, color);
                SDL_Surface *s = compress_frame(-1, 1);
                redraw_surface((GtkDrawingArea *)widget, s);
                break;
            case SELECT:
                selection_press(event->x, event->y, win_x, win_y, widget);
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
        GdkRGBA *p = eyedropper(event->x, event->y, win_x, win_y);
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
    int h = win_y;
    int w = win_x;

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
                selection_motion(event->x, event->y, win_x, win_y, widget);

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
    int h = win_y;
    int w = win_x;
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
                selection_release(event->x, event->y, win_x, win_y, widget);
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
    g_signal_connect(window, "key_press_event", G_CALLBACK(keyboard_inputs), drawing_area);
    g_signal_connect(window, "key_release_event", G_CALLBACK(get_key_release), NULL);
    colorselect = color_select;
    return;
}
