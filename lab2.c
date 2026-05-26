#include <X11/Xlib.h>
#include <X11/Xutil.h>
#include <stdio.h>
#include <stdlib.h>

int main() {

    Display *display;
    Window window;
    XEvent event;
    GC gc;
    int drawing = 0;
    int last_x = 0;
    int last_y = 0;
    display = XOpenDisplay(NULL);
    if (display == NULL) {

        printf("Не удалось подключиться к X-серверу\n");

        return 1;
    }

    int screen = DefaultScreen(display);

    window = XCreateSimpleWindow(

        display,
        RootWindow(display, screen),
        100, 100,
        800, 600,
        1,
        BlackPixel(display, screen),
        WhitePixel(display, screen)
    );

    XSelectInput(
        display,
        window,

        ExposureMask |
        ButtonPressMask |
        ButtonReleaseMask |
        PointerMotionMask |
        KeyPressMask
    );

    XStoreName(display, window, "LAB 2 XLIB");
    gc = XCreateGC(display, window, 0, NULL);
    XSetForeground(
        display,
        gc,
        BlackPixel(display, screen)
    );
    XSetLineAttributes(
        display,
        gc,
        3,
        LineSolid,
        CapRound,
        JoinRound          
    );

    XMapWindow(display, window);
    while (1) {
        XNextEvent(display, &event);
        if (event.type == Expose) {
            XDrawString(
                display,
                window,
                gc,
                10,
                20,
                "Рисуйте мышью. ESC - выход",
                31
            );
        }
        if (event.type == ButtonPress) {
            if (event.xbutton.button == Button1) {
                drawing = 1;
                last_x = event.xbutton.x;
                last_y = event.xbutton.y;
                printf(
                    "Начало рисования: %d %d\n",
                    last_x,
                    last_y
                );
            }
        }
        if (event.type == MotionNotify) {
            if (drawing) {
                int current_x = event.xmotion.x;
                int current_y = event.xmotion.y;
                XDrawLine(
                    display,
                    window,
                    gc,
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
            if (event.xbutton.button == Button1) {
                drawing = 0;
                printf("Конец рисования\n");
            }
        }
        if (event.type == KeyPress) {
            KeySym key;
            key = XLookupKeysym(
                &event.xkey,
                0
            );
            if (key == XK_Escape) {
                printf("Выход...\n");
                break;
            }
        }
    }
    XFreeGC(display, gc);
    XDestroyWindow(display, window);
    XCloseDisplay(display);
    return 0;
}
