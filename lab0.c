#include <X11/Xutil.h>
#include <X11/Xlib.h>
#include <X11/keysym.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int main() {
    Display *display;
    Window window;
    XEvent event;
    GC gc;
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
        KeyPressMask |
        ButtonPressMask
    );

    XStoreName(display, window, "Лаба 1 XLIB");
    gc = XCreateGC(display, window, 0, NULL);
    XMapWindow(display, window);

    while (1) {
        XNextEvent(display, &event);
        /*
            =========================
            СОБЫТИЕ ПЕРЕРИСОВКИ
            =========================
        */
        if (event.type == Expose) {
            XDrawString(
                display,
                window,
                gc,
                50, 50,
                "Привет XLIB!",
                strlen("Привет XLIB!")
            );
            
            XDrawRectangle(
                display,
                window,
                gc,
                100, 100,
                200, 60
            );
            XDrawString(
                display,
                window,
                gc,
                160, 135,
                "КНОПКА",
                strlen("КНОПКА")
            );
        }

        /*
            =========================
            СОБЫТИЕ КЛАВИАТУРЫ
            =========================
        */
        if (event.type == KeyPress) {
            char text[10];
            KeySym key;
            XLookupString(
                &event.xkey,
                text,
                sizeof(text),
                &key,
                NULL
            );
            printf("Нажата клавиша: %s\n", text);
            if (key == XK_Escape) {
                printf("Выход...\n");
                break;
            }
        }

        /*
            =========================
            СОБЫТИЕ МЫШИ
            =========================
        */
        if (event.type == ButtonPress) {
            int x = event.xbutton.x;
            int y = event.xbutton.y;
            printf("Клик мыши: %d %d\n", x, y);
            if (x >= 100 &&
                x <= 300 &&
                y >= 100 &&
                y <= 160)
            {
                printf("Кнопка нажата!\n");
            }
        }
    }

    XFreeGC(display, gc);
    XDestroyWindow(display, window);
    XCloseDisplay(display);
    return 0;
}
