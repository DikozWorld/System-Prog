#include <X11/Xlib.h>
#include <X11/Xutil.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// Параметры кнопки
#define BUTTON_X 100
#define BUTTON_Y 100
#define BUTTON_WIDTH 200
#define BUTTON_HEIGHT 60

int main() {

    Display *display;
    Window window;
    GC gc;
    XEvent event;

    int screen;

    // Состояние кнопки
    int button_pressed = 0;

    // Подключение к X серверу
    display = XOpenDisplay(NULL);

    if (display == NULL) {
        printf("Ошибка подключения к X серверу\n");
        return 1;
    }

    screen = DefaultScreen(display);

    // Создание окна
    window = XCreateSimpleWindow(
        display,
        RootWindow(display, screen),
        200, 200,
        600, 400,
        1,
        BlackPixel(display, screen),
        WhitePixel(display, screen)
    );

    // Подписка на события
    XSelectInput(
        display,
        window,
        ExposureMask |
        ButtonPressMask |
        KeyPressMask |
        StructureNotifyMask
    );

    // Заголовок окна
    XStoreName(display, window, "XLIB Button");

    // Создание контекста рисования
    gc = XCreateGC(display, window, 0, NULL);

    // Отображение окна
    XMapWindow(display, window);

    // Цвета
    Colormap colormap = DefaultColormap(display, screen);

    XColor blueColor;
    XColor redColor;

    XAllocNamedColor(display, colormap, "blue", &blueColor, &blueColor);
    XAllocNamedColor(display, colormap, "red", &redColor, &redColor);

    // Для корректного закрытия окна
    Atom wmDeleteMessage = XInternAtom(display, "WM_DELETE_WINDOW", False);
    XSetWMProtocols(display, window, &wmDeleteMessage, 1);

    // Главный цикл
    while (1) {

        XNextEvent(display, &event);

        // Закрытие окна
        if (event.type == ClientMessage) {

            if ((Atom)event.xclient.data.l[0] == wmDeleteMessage) {
                break;
            }
        }

        // Перерисовка
        if (event.type == Expose) {

            // Цвет кнопки
            if (button_pressed) {
                XSetForeground(display, gc, redColor.pixel);
            } else {
                XSetForeground(display, gc, blueColor.pixel);
            }

            // Рисуем кнопку
            XFillRectangle(
                display,
                window,
                gc,
                BUTTON_X,
                BUTTON_Y,
                BUTTON_WIDTH,
                BUTTON_HEIGHT
            );

            // Цвет текста
            XSetForeground(display, gc, WhitePixel(display, screen));

            // Текст кнопки
            char text[] = "BUTTON";

            XDrawString(
                display,
                window,
                gc,
                BUTTON_X + 65,
                BUTTON_Y + 35,
                text,
                strlen(text)
            );
        }

        // Нажатие мыши
        if (event.type == ButtonPress) {

            int mouse_x = event.xbutton.x;
            int mouse_y = event.xbutton.y;

            printf("Клик мыши: %d %d\n", mouse_x, mouse_y);

            // Проверка попадания в кнопку
            if (mouse_x >= BUTTON_X &&
                mouse_x <= BUTTON_X + BUTTON_WIDTH &&
                mouse_y >= BUTTON_Y &&
                mouse_y <= BUTTON_Y + BUTTON_HEIGHT)
            {

                printf("Кнопка нажата!\n");

                // Меняем состояние
                button_pressed = !button_pressed;

                // Принудительная перерисовка
                XClearWindow(display, window);
            }
        }

        // Выход по Escape
        if (event.type == KeyPress) {

            KeySym key;

            key = XLookupKeysym(&event.xkey, 0);

            if (key == XK_Escape) {
                break;
            }
        }
    }

    // Освобождение ресурсов
    XFreeGC(display, gc);
    XDestroyWindow(display, window);
    XCloseDisplay(display);

    return 0;
}
