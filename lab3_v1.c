#include <X11/Xlib.h>
#include <X11/keysym.h>

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

Display *display;
Window main_window;
Window red_button;
Window green_button;
Window blue_button;
GC draw_gc;
GC ui_gc;
XColor red_color;
XColor green_color;
XColor blue_color;
int drawing = 0;
int last_x = 0;
int last_y = 0;

void draw_button(
    Window button,
    const char *text
) {
    XDrawRectangle(
        display,
        button,
        ui_gc,
        0,
        0,
        98,
        48
    );

    XDrawString(
        display,
        button,
        ui_gc,

        20,
        28,

        text,
        strlen(text)
    );
}

int main() {
    display = XOpenDisplay(NULL);
    if (!display) {
        printf("Ошибка подключения к X11\n");
        return 1;
    }

    int screen =
        DefaultScreen(display);
    main_window =
        XCreateSimpleWindow(
            display,
            RootWindow(display, screen),
            100,
            100,
            1000,
            700,
            1,
            BlackPixel(display, screen),
            WhitePixel(display, screen)
        );
    red_button =
        XCreateSimpleWindow(
            display,
            main_window,
            20,
            20,
            100,
            50,
            1,
            BlackPixel(display, screen),
            WhitePixel(display, screen)
        );

    green_button =
        XCreateSimpleWindow(
            display,
            main_window,
            140,
            20,
            100,
            50,
            1,
            BlackPixel(display, screen),
            WhitePixel(display, screen)
        );

    blue_button =
        XCreateSimpleWindow(
            display,
            main_window,
            260,
            20,
            100,
            50,
            1,
            BlackPixel(display, screen),
            WhitePixel(display, screen)
        );


    XSelectInput(
        display,
        main_window,
        ExposureMask |
        ButtonPressMask |
        ButtonReleaseMask |
        PointerMotionMask |
        KeyPressMask
    );

    XSelectInput(
        display,
        red_button,
        ExposureMask |
        ButtonPressMask
    );

    XSelectInput(
        display,
        green_button,
        ExposureMask |
        ButtonPressMask
    );

    XSelectInput(
        display,
        blue_button,
        ExposureMask |
        ButtonPressMask
    );

    draw_gc =
        XCreateGC(
            display,
            main_window,
            0,
            NULL
        );

    ui_gc =
        XCreateGC(
            display,
            main_window,
            0,
            NULL
        );

    XSetForeground(
        display,
        ui_gc,
        BlackPixel(display, screen)
    );

    XSetLineAttributes(

        display,
        draw_gc,
        8,
        LineSolid,
        CapRound,
        JoinRound
    );


    Colormap colormap =
        DefaultColormap(display, screen);
    XAllocNamedColor(
        display,
        colormap,
        "red",
        &red_color,
        &red_color
    );

    XAllocNamedColor(
        display,
        colormap,
        "lime",
        &green_color,
        &green_color
    );
    XAllocNamedColor(
        display,
        colormap,
        "blue",
        &blue_color,
        &blue_color
    );

    XSetForeground(
        display,
        draw_gc,
        BlackPixel(display, screen)
    );
    XStoreName(
        display,
        main_window,
        "Lab 3"
    );
    XMapWindow(display, main_window);
    XMapWindow(display, red_button);
    XMapWindow(display, green_button);
    XMapWindow(display, blue_button);

    XEvent event;
    while (1) {

        XNextEvent(
            display,
            &event
        );
        if (event.type == Expose) {
            if (
                event.xany.window ==
                main_window
            ) {
                XDrawString(

                    display,
                    main_window,
                    ui_gc,

                    20,
                    100,
                    "Рисуйте мышью ниже кнопок",
                    30
                );
            }
            else if (
                event.xany.window ==
                red_button
            ) {
                draw_button(
                    red_button,
                    "RED"
                );
            }
            else if (
                event.xany.window ==
                green_button
            ) {
                draw_button(
                    green_button,
                    "GREEN"
                );
            }
            else if (
                event.xany.window ==
                blue_button
            ) {
                draw_button(
                    blue_button,
                    "BLUE"
                );
            }
        }

        if (event.type == ButtonPress) {
            if (
                event.xany.window ==
                red_button
            ) {
                printf(
                    "Выбран красный цвет\n"
                );
                XSetForeground(
                    display,
                    draw_gc,
                    red_color.pixel
                );
            }
            else if (
                event.xany.window ==
                green_button
            ) {
                printf(
                    "Выбран зеленый цвет\n"
                );
                XSetForeground(
                    display,
                    draw_gc,
                    green_color.pixel
                );
            }
            else if (
                event.xany.window ==
                blue_button
            ) {
                printf(
                    "Выбран синий цвет\n"
                );
                XSetForeground(
                    display,
                    draw_gc,
                    blue_color.pixel
                );
            }
            else if (
                event.xany.window ==
                main_window
            ) {
                if (
                    event.xbutton.button ==
                    Button1
                ) {
                    if (
                        event.xbutton.y > 90
                    ) {
                        drawing = 1;
                        last_x =
                            event.xbutton.x;
                        last_y =
                            event.xbutton.y;
                    }
                }
            }
        }
        if (event.type == MotionNotify) {
            if (
                drawing &&
                event.xany.window ==
                main_window
            ) {
                int current_x =
                    event.xmotion.x;
                int current_y =
                    event.xmotion.y;
                XDrawLine(
                    display,
                    main_window,
                    draw_gc,
                    last_x,
                    last_y,
                    current_x,
                    current_y
                );
                last_x = current_x;
                last_y = current_y;
            }
        }
        if (event.type == ButtonRelease) {
            drawing = 0;
        }
        if (event.type == KeyPress) {
            KeySym key =
                XLookupKeysym(
                    &event.xkey,
                    0
                );
            if (
                key == XK_Escape
            ) {
                break;
            }
        }
    }

    XFreeGC(display, draw_gc);
    XFreeGC(display, ui_gc);
    XDestroyWindow(display, red_button);
    XDestroyWindow(display, green_button);
    XDestroyWindow(display, blue_button);
    XDestroyWindow(display, main_window);
    XCloseDisplay(display);

    return 0;
}
