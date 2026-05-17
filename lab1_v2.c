#include <X11/Xlib.h>
#include <X11/Xutil.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int main() {
    Display *display;
    Window root, main_window, button_window;
    int screen;
    XEvent event;

    // 1. Подключаемся к X-серверу
    display = XOpenDisplay(NULL);
    if (display == NULL) {
        fprintf(stderr, "Не удалось открыть дисплей\n");
        exit(1);
    }

    screen = DefaultScreen(display);
    root = RootWindow(display, screen);

    // 2. Создаем главное окно
    // Координаты (x,y) = (10,10), размер 400x300
    main_window = XCreateSimpleWindow(display, root, 10, 10, 400, 300, 1,
                                      BlackPixel(display, screen), 
                                      WhitePixel(display, screen));

    // 3. Создаем "кнопку" (дочернее окно внутри главного)
    // Координаты относительно главного окна: x=120, y=100, размер 150x50
    button_window = XCreateSimpleWindow(display, main_window, 120, 100, 150, 50, 2,
                                        BlackPixel(display, screen), 
                                        LightGrayPixel(display, screen));

    // 4. Указываем, какие события нам интересны
    // Для главного окна - нажатия клавиш (чтобы выйти по любой кнопке)
    XSelectInput(display, main_window, KeyPressMask);
    
    // Для кнопки - клики мыши (нажатие и отпускание) и перерисовка (Expose)
    XSelectInput(display, button_window, ExposureMask | ButtonPressMask | ButtonReleaseMask | EnterWindowMask | LeaveWindowMask);

    // 5. Выводим окна на экран
    XMapWindow(display, main_window);
    XMapSubwindows(display, main_window); // Показываем все дочерние окна

    // Получаем графический контекст для рисования текста
    GC gc = DefaultGC(display, screen);
    const char *btn_text = "Click Me!";

    // 6. Главный цикл обработки событий
    while (1) {
        XNextEvent(display, &event);

        // Если нужно перерисовать кнопку (например, при первом появлении окна)
        if (event.type == Expose && event.xany.window == button_window) {
            // Рисуем текст по координатам (40, 30) внутри окна-кнопки
            XDrawString(display, button_window, gc, 40, 30, btn_text, strlen(btn_text));
        }

        // Если мышь зашла в пределы кнопки (эффект наведения)
        if (event.type == EnterNotify && event.xcrossing.window == button_window) {
            XSetWindowBorderWidth(display, button_window, 4); // Делаем рамку толще
        }
        
        // Если мышь покинула пределы кнопки
        if (event.type == LeaveNotify && event.xcrossing.window == button_window) {
            XSetWindowBorderWidth(display, button_window, 2); // Возвращаем обычную рамку
        }

        // Если на кнопку нажали
        if (event.type == ButtonPress && event.xany.window == button_window) {
            printf("Кнопка нажата!\n");
        }

        // Если нажали любую клавишу на клавиатуре в главном окне — выходим
        if (event.type == KeyPress) {
            printf("Выход из программы...\n");
            break;
        }
    }

    // 7. Чистим за собой
    XCloseDisplay(display);
    return 0;
}
