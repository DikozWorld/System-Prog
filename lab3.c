#include <X11/Xlib.h>
#include <X11/Xutil.h>
#include <X11/keysym.h>

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/*
    ==================================================
    ЛАБА 3
    Простейший графический редактор
    Несколько цветов
    ==================================================
*/

Display *display;

Window main_window;

Window red_button;
Window green_button;
Window blue_button;

GC gc;

/*
    Текущий цвет рисования
*/

unsigned long current_color;

/*
    Цвета X11
*/

XColor red_color;
XColor green_color;
XColor blue_color;

/*
    Флаг рисования
*/

int drawing = 0;

/*
    Предыдущая позиция мыши
*/

int last_x = 0;
int last_y = 0;

/*
    ==================================================
    РИСОВАНИЕ КНОПОК
    ==================================================
*/

void draw_button(
    Window button,
    const char *text
) {

    /*
        Кнопки всегда рисуем чёрным цветом
    */

    XSetForeground(
        display,
        gc,
        BlackPixel(
            display,
            DefaultScreen(display)
        )
    );

    /*
        Рамка
    */

    XDrawRectangle(
        display,
        button,
        gc,

        0, 0,

        98, 48
    );

    /*
        Текст
    */

    XDrawString(
        display,
        button,
        gc,

        25,
        28,

        text,
        strlen(text)
    );
}

/*
    ==================================================
    MAIN
    ==================================================
*/

int main() {

    /*
        ==========================================
        ПОДКЛЮЧЕНИЕ К X11
        ==========================================
    */

    display = XOpenDisplay(NULL);

    if (display == NULL) {

        printf("Ошибка подключения к X11\n");

        return 1;
    }

    int screen = DefaultScreen(display);

    /*
        ==========================================
        ГЛАВНОЕ ОКНО
        ==========================================
    */

    main_window = XCreateSimpleWindow(

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

    red_button = XCreateSimpleWindow(

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

    green_button = XCreateSimpleWindow(

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

    blue_button = XCreateSimpleWindow(

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
        ButtonReleaseMask
    );

    XSelectInput(

        display,

        green_button,

        ExposureMask |
        ButtonReleaseMask
    );

    XSelectInput(

        display,

        blue_button,

        ExposureMask |
        ButtonReleaseMask
    );

    /*
        ==========================================
        GC
        ==========================================
    */

    gc = XCreateGC(
        display,
        main_window,
        0,
        NULL
    );

    /*
        ==========================================
        НАСТРОЙКА ЛИНИЙ
        ==========================================
    */

    XSetLineAttributes(

        display,
        gc,

        3,

        LineSolid,
        CapRound,
        JoinRound
    );

    /*
        ==========================================
        ПОЛУЧАЕМ ЦВЕТА
        ==========================================
    */

    Colormap colormap;

    colormap = DefaultColormap(
        display,
        screen
    );

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

        "green",

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

    /*
        ==========================================
        ЦВЕТ ПО УМОЛЧАНИЮ
        ==========================================
    */

    current_color = BlackPixel(
        display,
        screen
    );

    XSetForeground(
        display,
        gc,
        current_color
    );

    /*
        ==========================================
        ИМЯ ОКНА
        ==========================================
    */

    XStoreName(
        display,
        main_window,
        "Лаба 3 - Paint Colors"
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

            /*
                Главное окно
            */

            if (
                event.xany.window ==
                main_window
            ) {

                /*
                    Текст интерфейса рисуем чёрным
                */

                XSetForeground(
                    display,
                    gc,
                    BlackPixel(display, screen)
                );

                XDrawString(

                    display,
                    main_window,
                    gc,

                    20,
                    100,

                    "Рисуйте мышью. ESC - выход",

                    31
                );

                /*
                    Возвращаем текущий цвет
                */

                XSetForeground(
                    display,
                    gc,
                    current_color
                );
            }

            /*
                КНОПКИ
            */

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
                Рисуем
                только в главном окне
            */

            if (
                event.xany.window ==
                main_window
            ) {

                if (
                    event.xbutton.button ==
                    Button1
                ) {

                    drawing = 1;

                    last_x =
                        event.xbutton.x;

                    last_y =
                        event.xbutton.y;
                }
            }
        }

        /*
            ==========================================
            MOTION
            ==========================================
        */

        if (event.type == MotionNotify) {

            if (drawing) {

                int current_x =
                    event.xmotion.x;

                int current_y =
                    event.xmotion.y;

                /*
                    Устанавливаем цвет рисования
                */

                XSetForeground(
                    display,
                    gc,
                    current_color
                );

                /*
                    Рисуем линию
                */

                XDrawLine(

                    display,
                    main_window,
                    gc,

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

            /*
                RED
            */

            if (
                event.xany.window ==
                red_button
            ) {

                current_color =
                    red_color.pixel;

                XSetForeground(
                    display,
                    gc,
                    current_color
                );

                printf("Выбран RED\n");
            }

            /*
                GREEN
            */

            else if (
                event.xany.window ==
                green_button
            ) {

                current_color =
                    green_color.pixel;

                XSetForeground(
                    display,
                    gc,
                    current_color
                );

                printf("Выбран GREEN\n");
            }

            /*
                BLUE
            */

            else if (
                event.xany.window ==
                blue_button
            ) {

                current_color =
                    blue_color.pixel;

                XSetForeground(
                    display,
                    gc,
                    current_color
                );

                printf("Выбран BLUE\n");
            }
        }

        /*
            ==========================================
            КЛАВИАТУРА
            ==========================================
        */

        if (event.type == KeyPress) {

            KeySym key;

            key = XLookupKeysym(
                &event.xkey,
                0
            );

            /*
                ESC
            */

            if (key == XK_Escape) {

                printf("Выход...\n");

                break;
            }
        }
    }

    /*
        ==========================================
        ОСВОБОЖДАЕМ РЕСУРСЫ
        ==========================================
    */

    XFreeGC(
        display,
        gc
    );

    XDestroyWindow(
        display,
        red_button
    );

    XDestroyWindow(
        display,
        green_button
    );

    XDestroyWindow(
        display,
        blue_button
    );

    XDestroyWindow(
        display,
        main_window
    );

    XCloseDisplay(
        display
    );

    return 0;
}
