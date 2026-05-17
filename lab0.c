#include <X11/Xlib.h>     // Основная библиотека Xlib
#include <X11/keysym.h>   // Константы клавиш (например XK_Escape)
#include <stdio.h>        // printf
#include <stdlib.h>       // exit
#include <string.h>       // strlen

int main() {

    /*
        Display* — это УКАЗАТЕЛЬ на структуру Display.

        Display хранит:
        - соединение с X-сервером
        - очередь событий
        - информацию об экранах
        - настройки X11

        Через display происходит почти ВСЁ взаимодействие с X11.
    */
    Display *display;

    /*
        Window — НЕ структура.

        Window — это просто ID окна на X-сервере.

        Настоящее окно хранится внутри X11.
        У нас есть только его идентификатор.
    */
    Window window;

    /*
        XEvent — структура/union для хранения события.

        В event могут лежать:
        - событие клавиатуры
        - событие мыши
        - событие перерисовки
        - и т.д.

        Какое именно событие внутри —
        определяется через event.type
    */
    XEvent event;

    /*
        GC = Graphics Context

        Это объект с настройками рисования:
        - цвет
        - шрифт
        - толщина линии
        - стиль рисования

        GC можно представить как "кисть".
    */
    GC gc;

    /*
        Подключаемся к X-серверу.

        NULL означает:
        "используй DISPLAY из переменных окружения"

        Обычно это:
        :0
    */
    display = XOpenDisplay(NULL);

    /*
        Если подключение не удалось —
        XOpenDisplay возвращает NULL.
    */
    if (display == NULL) {

        printf("Не удалось подключиться к X-серверу\n");

        return 1;
    }

    /*
        Получаем номер экрана.

        В X11 может быть несколько экранов.
        Обычно основной экран имеет номер 0.

        display передаётся потому,
        что экраны принадлежат X-серверу.
    */
    int screen = DefaultScreen(display);

    /*
        Создаём окно.

        Параметры:
        display                     -> подключение к X11
        RootWindow(...)             -> родительское окно
        100,100                     -> позиция окна
        800,600                     -> размер окна
        1                           -> толщина рамки
        BlackPixel(...)             -> цвет рамки
        WhitePixel(...)             -> цвет фона
    */
    window = XCreateSimpleWindow(

        display,

        RootWindow(display, screen),

        100, 100,

        800, 600,

        1,

        BlackPixel(display, screen),

        WhitePixel(display, screen)
    );

    /*
        Подписываемся на события.

        Важно:
        X11 НЕ отправляет все события автоматически.

        Нужно указать:
        какие события мы хотим получать.

        ExposureMask       -> события перерисовки
        KeyPressMask       -> нажатие клавиш
        ButtonPressMask    -> нажатие мыши
    */
    XSelectInput(

        display,

        window,

        ExposureMask |
        KeyPressMask |
        ButtonPressMask
    );

    /*
        Устанавливаем заголовок окна.
    */
    XStoreName(display, window, "XLIB Example");

    /*
        Создаём Graphics Context.

        display -> какой X-сервер
        window  -> для какого окна
    */
    gc = XCreateGC(display, window, 0, NULL);

    /*
        Делаем окно видимым.

        После XCreateSimpleWindow окно существует,
        но оно ещё скрыто.

        XMapWindow сообщает X-серверу:
        "покажи окно"
    */
    XMapWindow(display, window);

    /*
        Главный цикл программы.

        GUI-приложения почти всегда работают
        через бесконечный event loop.
    */
    while (1) {

        /*
            Получаем следующее событие.

            display -> откуда получать события
            &event  -> куда записать событие

            Важно:
            &event = адрес структуры event.

            Мы передаём адрес,
            чтобы Xlib могла изменить
            ОРИГИНАЛЬНУЮ структуру.
        */
        XNextEvent(display, &event);

        /*
            Проверяем тип события.
        */

        /*
            =========================
            СОБЫТИЕ ПЕРЕРИСОВКИ
            =========================
        */
        if (event.type == Expose) {

            /*
                Expose возникает:
                - после открытия окна
                - после разворачивания
                - после перекрытия другим окном

                В XLIB окно НЕ хранит рисунок автоматически.

                Поэтому при Expose
                нужно рисовать интерфейс заново.
            */

            /*
                Рисуем строку текста.

                Параметры:
                display -> X-сервер
                window  -> где рисовать
                gc      -> чем рисовать
                50,50   -> координаты
                строка
                длина строки
            */
            XDrawString(

                display,
                window,
                gc,

                50, 50,

                "Hello XLIB!",

                strlen("Hello XLIB!")
            );

            /*
                Рисуем прямоугольник.

                Это будет "кнопка".
            */
            XDrawRectangle(

                display,
                window,
                gc,

                100, 100,

                200, 60
            );

            /*
                Текст внутри кнопки.
            */
            XDrawString(

                display,
                window,
                gc,

                160, 135,

                "BUTTON",

                strlen("BUTTON")
            );
        }

        /*
            =========================
            СОБЫТИЕ КЛАВИАТУРЫ
            =========================
        */
        if (event.type == KeyPress) {

            /*
                text — массив символов,
                куда будет записана клавиша.
            */
            char text[10];

            /*
                KeySym — специальный код клавиши.

                Например:
                XK_Escape
                XK_Left
                XK_Right
            */
            KeySym key;

            /*
                Преобразуем событие клавиатуры
                в символ.

                &event.xkey -> адрес структуры события клавиатуры
                text         -> массив для символа
                sizeof(text) -> размер массива
                &key         -> сюда запишется KeySym
            */
            XLookupString(

                &event.xkey,

                text,

                sizeof(text),

                &key,

                NULL
            );

            printf("Нажата клавиша: %s\n", text);

            /*
                Если нажали Escape —
                выходим.
            */
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

            /*
                Координаты клика мыши.
            */
            int x = event.xbutton.x;
            int y = event.xbutton.y;

            printf("Клик мыши: %d %d\n", x, y);

            /*
                Проверяем:
                попала ли мышь внутрь кнопки.
            */
            if (x >= 100 &&
                x <= 300 &&
                y >= 100 &&
                y <= 160)
            {
                printf("Кнопка нажата!\n");
            }
        }
    }

    /*
        Освобождаем Graphics Context.
    */
    XFreeGC(display, gc);

    /*
        Уничтожаем окно.
    */
    XDestroyWindow(display, window);

    /*
        Закрываем соединение с X-сервером.
    */
    XCloseDisplay(display);

    return 0;
}
