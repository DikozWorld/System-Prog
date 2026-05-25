#include <X11/Xlib.h>
#include <X11/keysym.h>

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/*
    ==========================================
    ЛАБА 3
    Paint с несколькими цветами
    ==========================================
*/

Display *display;

Window main_window;

Window red_button;
Window green_button;
Window blue_button;

/*
    GC для рисования
*/

GC draw_gc;

/*
    GC для интерфейса
*/

GC ui_gc;

/*
    Цвета
*/

XColor red_color;
XColor green_color;
XColor blue_color;

/*
    Рисуем ли сейчас
*/

int drawing = 0;

/*
    Последняя точка
*/

int last_x = 0;
int last_y = 0;

/*
    ==========================================
    РИСУЕМ КНОПКУ
    ==========================================
*/

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

/*
    ==========================================
    MAIN
    ==========================================
*/

int main() {

    /*
        ==========================================
        ПОДКЛЮЧЕНИЕ К X11
        ==========================================
    */

    display = XOpenDisplay(NULL);

    if (!display) {

        printf("Ошибка подключения к X11\n");

        return 1;
    }

    int screen =
        DefaultScreen(display);

    /*
        ==========================================
        ГЛАВНОЕ ОКНО
        ==========================================
    */

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

    /*
        ==========================================
        КНОПКИ
        ==========================================
    */

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

    /*
        ==========================================
        СОБЫТИЯ
        ==========================================
    */

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

    /*
        ==========================================
        СОЗДАЁМ GC
        ==========================================
    */

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

    /*
        UI всегда чёрный
    */

    XSetForeground(

        display,
        ui_gc,

        BlackPixel(display, screen)
    );

    /*
        Толщина линии
    */

    XSetLineAttributes(

        display,
        draw_gc,

        8,

        LineSolid,
        CapRound,
        JoinRound
    );

    /*
        ==========================================
        ЦВЕТА
        ==========================================
    */

    Colormap colormap =
        DefaultColormap(display, screen);

    /*
        RED
    */

    XAllocNamedColor(

        display,
        colormap,

        "red",

        &red_color,
        &red_color
    );

    /*
        GREEN
    */

    XAllocNamedColor(

        display,
        colormap,

        "lime",

        &green_color,
        &green_color
    );

    /*
        BLUE
    */

    XAllocNamedColor(

        display,
        colormap,

        "blue",

        &blue_color,
        &blue_color
    );

    /*
        Цвет по умолчанию
    */

    XSetForeground(

        display,
        draw_gc,

        BlackPixel(display, screen)
    );

    /*
        ==========================================
        ИМЯ ОКНА
        ==========================================
    */

    XStoreName(
        display,
        main_window,
        "Lab 3"
    );

    /*
        ==========================================
        ПОКАЗЫВАЕМ ОКНА
        ==========================================
    */

    XMapWindow(display, main_window);

    XMapWindow(display, red_button);

    XMapWindow(display, green_button);

    XMapWindow(display, blue_button);

    /*
        ==========================================
        EVENT LOOP
        ==========================================
    */

    XEvent event;

    while (1) {

        XNextEvent(
            display,
            &event
        );

        /*
            ==========================================
            EXPOSE
            ==========================================
        */

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

        /*
            ==========================================
            BUTTON PRESS
            ==========================================
        */

        if (event.type == ButtonPress) {

            /*
                RED BUTTON
            */

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

            /*
                GREEN BUTTON
            */

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

            /*
                BLUE BUTTON
            */

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

            /*
                РИСОВАНИЕ
            */

            else if (
                event.xany.window ==
                main_window
            ) {

                if (
                    event.xbutton.button ==
                    Button1
                ) {

                    /*
                        Не рисуем поверх кнопок
                    */

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

        /*
            ==========================================
            MOTION
            ==========================================
        */

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

                /*
                    Обновляем координаты
                */

                last_x = current_x;

                last_y = current_y;
            }
        }

        /*
            ==========================================
            BUTTON RELEASE
            ==========================================
        */

        if (event.type == ButtonRelease) {

            drawing = 0;
        }

        /*
            ==========================================
            КЛАВИАТУРА
            ==========================================
        */

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

    /*
        ==========================================
        ОСВОБОЖДАЕМ РЕСУРСЫ
        ==========================================
    */

    XFreeGC(display, draw_gc);

    XFreeGC(display, ui_gc);

    XDestroyWindow(display, red_button);

    XDestroyWindow(display, green_button);

    XDestroyWindow(display, blue_button);

    XDestroyWindow(display, main_window);

    XCloseDisplay(display);

    return 0;
}
