#include <X11/Xutil.h>
#include <X11/Xlib.h>
#include <X11/keysym.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int main() {
    Display *display;
    Window window;
    Window button_window;
    XEvent event;
    GC gc;
    
    display = XOpenDisplay(NULL);
    if (display == NULL) {

        printf("Не удалось подключиться к X-серверу\n");

        return 1;
    }

    int screen = DefaultScreen(display);
    
    // СОЗДАЁМ ГЛАВНОЕ ОКНО
    window = XCreateSimpleWindow(
        display,
        RootWindow(display, screen),
        100, 100,
        800, 600,
        1,
        BlackPixel(display, screen),
        WhitePixel(display, screen)
    );

    // СОЗДАЁМ ДОЧЕРНЕЕ ОКНО ДЛЯ КНОПКИ 
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
    
    // НОВОЕ: выбираем события для окна кнопки
    XSelectInput(
        display,
        button_window,
        ExposureMask |
        ButtonPressMask |
        ButtonReleaseMask
    );

    XStoreName(display, window, "lab 1 XLIB");
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
            // Проверяем, какое окно перерисовывается
            if (event.xany.window == window) {
                // Рисуем в главном окне
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
                // Рисуем в окне кнопки
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
                int x = event.xbutton.x;
                int y = event.xbutton.y;

                /*
                    Проверяем:
                    находится ли курсор
                    внутри кнопки
                */
                if (x >= 0 && x <= 199 &&
                    y >= 0 && y <= 59)
                {
                    printf("Кнопка нажата!\n");
                }
    }
}

    XFreeGC(display, gc);
    XDestroyWindow(display, button_window); 
    XDestroyWindow(display, window);
    XCloseDisplay(display);
    return 0;
}
