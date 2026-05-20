#include <X11/Xutil.h>
#include <X11/Xlib.h>
#include <X11/keysym.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int main() {
    Display *display;
    Window window;
    Window button_window;  // НОВОЕ: окно для кнопки
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

    button_window = XCreateSimpleWindow(
        display,
        window,
        100, 100,
        200, 60,
        1,
        BlackPixel(display, screen),
        WhitePixel(display, screen)
    );

    XSelectInput(
        display,
        window,
        ExposureMask |
        KeyPressMask |
        ButtonPressMask |
        ButtonReleaseMask
    );
    
    XSelectInput(
        display,
        button_window,
        ExposureMask |
        ButtonPressMask |
        ButtonReleaseMask
    );

    XStoreName(display, window, "LAB 1 XLIB");
    gc = XCreateGC(display, window, 0, NULL);
    
    XMapWindow(display, window);
    XMapWindow(display, button_window);

    while (1) {
        XNextEvent(display, &event);
        
        /*
            =========================
            СОБЫТИЕ ПЕРЕРИСОВКИ
            =========================
        */
        if (event.type == Expose) {
            if (event.xany.window == window) {
                XDrawString(
                    display,
                    window,
                    gc,
                    50, 50,
                    "Hello XLIB!",
                    strlen("Hello XLIB!")
                );
            }
            else if (event.xany.window == button_window) {
                XDrawRectangle(
                    display,
                    button_window,
                    gc,
                    0, 0,
                    199, 59
                );
                XDrawString(
                    display,
                    button_window,
                    gc,
                    70, 35,
                    "BUTTON",
                    strlen("BUTTON")
                );
            }
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
        if (event.type == ButtonRelease) {
            if (event.xany.window == button_window) {
                printf("Кнопка нажата!\n");
            }
            else if (event.xany.window == window) {
                int x = event.xbutton.x;
                int y = event.xbutton.y;
                printf("Клик в главном окне: %d %d\n", x, y);
            }
        }
    }

    XFreeGC(display, gc);
    XDestroyWindow(display, button_window);
    XDestroyWindow(display, window);
    XCloseDisplay(display);
    return 0;
}
