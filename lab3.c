#include <X11/Xlib.h>
#include <X11/Xutil.h>
#include <X11/keysym.h>

#include <stdio.h>
#include <stdlib.h>

/*
    ==================================================
    ЛАБА 3
    Простейший графический редактор
    Несколько цветов
    ==================================================

    Что умеет программа:
    - рисование мышью
    - выбор цвета кнопками
    - несколько child windows
    - выход по ESC

    Цвета:
    - RED
    - GREEN
    - BLUE
*/

/*
    ==================================================
    ГЛОБАЛЬНЫЕ ПЕРЕМЕННЫЕ
    ==================================================
*/

/*
    Храним:
    подключение к X11
*/

Display *display;

/*
    Главное окно
*/

Window main_window;

/*
    Кнопки цветов
*/

Window red_button;
Window green_button;
Window blue_button;

/*
    Graphics Context
*/

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
    ФУНКЦИЯ:
    РИСОВАНИЕ КНОПКИ
    ==================================================
*/

void draw_button(
    Window button,
    const char *text
) {

    /*
        Рамка кнопки
    */

    XDrawRectangle(
        display,
        button,
        gc,

        0, 0,

        98, 48
    );

    /*
        Текст кнопки
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
        СОЗДАЁМ ГЛАВНОЕ ОКНО
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
        СОЗДАЁМ КНОПКИ ЦВЕТОВ
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
        ВЫБИРАЕМ СОБЫТИЯ
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

    /*
        Для кнопок:
        expose + mouse
    */

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
        СОЗДАЁМ GC
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
        НАСТРАИВАЕМ ЛИНИЮ
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
        ПОЛУЧАЕМ ЦВЕТА ИЗ X11
        ==========================================
    */

    Colormap colormap;

    colormap = DefaultColormap(
        display,
        screen
    );

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

        "green",

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
        ==========================================
        ЦВЕТ ПО УМОЛЧАНИЮ
        ==========================================
    */

    current_color = BlackPixel(
        display,
        screen
    );

    /*
        ==========================================
        НАЗВАНИЕ ОКНА
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
        ГЛАВНЫЙ ЦИКЛ
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

            if (event.xany.window == main_window) {

                XDrawString(

                    display,
                    main_window,
                    gc,

                    20,
                    100,

                    "Рисуйте мышью. ESC - выход",

                    31
                );
            }

            /*
                RED BUTTON
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

            /*
                GREEN BUTTON
            */

            else if (
                event.xany.window ==
                green_button
            ) {

                draw_button(
                    green_button,
                    "GREEN"
                );
            }

            /*
                BLUE BUTTON
            */

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
            НАЖАТИЕ МЫШИ
            ==========================================
        */

        if (event.type == ButtonPress) {

            /*
                Рисуем
                только в main_window
            */

            if (
                event.xany.window ==
                main_window
            ) {

                /*
                    Только ЛКМ
                */

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
            ДВИЖЕНИЕ МЫШИ
            ==========================================
        */

        if (event.type == MotionNotify) {

            /*
                Если рисуем
            */

            if (drawing) {

                /*
                    Текущая позиция
                */

                int current_x =
                    event.xmotion.x;

                int current_y =
                    event.xmotion.y;

                /*
                    Устанавливаем
                    текущий цвет
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
                    Обновляем
                    предыдущую точку
                */

                last_x = current_x;

                last_y = current_y;
            }
        }

        /*
            ==========================================
            ОТПУСКАНИЕ МЫШИ
            ==========================================
        */

        if (event.type == ButtonRelease) {

            /*
                Заканчиваем рисование
            */

            drawing = 0;

            /*
                RED BUTTON
            */

            if (
                event.xany.window ==
                red_button
            ) {

                current_color =
                    red_color.pixel;

                printf(
                    "Выбран RED\n"
                );
            }

            /*
                GREEN BUTTON
            */

            else if (
                event.xany.window ==
                green_button
            ) {

                current_color =
                    green_color.pixel;

                printf(
                    "Выбран GREEN\n"
                );
            }

            /*
                BLUE BUTTON
            */

            else if (
                event.xany.window ==
                blue_button
            ) {

                current_color =
                    blue_color.pixel;

                printf(
                    "Выбран BLUE\n"
                );
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
                ESC -> выход
            */

            if (key == XK_Escape) {

                printf(
                    "Выход...\n"
                );

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
