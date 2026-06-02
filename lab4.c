#include <X11/Xlib.h>
#include <X11/Xutil.h>

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define WIDTH 800
#define HEIGHT 600

#define LEFT_MARGIN 20
#define TOP_MARGIN 40

#define LINE_HEIGHT 20

#define MAX_PARAGRAPHS 100
#define MAX_PARAGRAPH_LEN 4096

/*
    =====================================
    ФУНКЦИЯ ВЫРАВНИВАНИЯ ПО ШИРИНЕ
    =====================================
*/

void drawJustifiedLine(
    Display *display,
    Window window,
    GC gc,
    XFontStruct *font,

    char words[][100],
    int word_count,

    int x,
    int y,
    int line_width
)
{
    if (word_count == 0)
        return;

    /*
        Последнее слово строки
    */

    if (word_count == 1)
    {
        XDrawString(
            display,
            window,
            gc,
            x,
            y,
            words[0],
            strlen(words[0])
        );
        return;
    }

    int words_width = 0;

    for (int i = 0; i < word_count; i++)
    {
        words_width +=
            XTextWidth(
                font,
                words[i],
                strlen(words[i])
            );
    }

    int gaps = word_count - 1;

    int free_space =
        line_width - words_width;

    int base_space =
        free_space / gaps;

    int extra =
        free_space % gaps;

    int current_x = x;

    for (int i = 0; i < word_count; i++)
    {
        XDrawString(
            display,
            window,
            gc,
            current_x,
            y,
            words[i],
            strlen(words[i])
        );

        current_x +=
            XTextWidth(
                font,
                words[i],
                strlen(words[i])
            );

        if (i < gaps)
        {
            current_x += base_space;

            if (extra > 0)
            {
                current_x++;
                extra--;
            }
        }
    }
}

/*
    =====================================
    ОТРИСОВКА АБЗАЦА
    =====================================
*/

int drawParagraph(
    Display *display,
    Window window,
    GC gc,
    XFontStruct *font,

    char *text,

    int x,
    int y,

    int width
)
{
    char copy[5000];

    strcpy(copy, text);

    char *token =
        strtok(copy, " ");

    char line[1000] = "";

    char words[100][100];

    int word_count = 0;

    while (token != NULL)
    {
        char test[1000];

        if (strlen(line) == 0)
        {
            strcpy(test, token);
        }
        else
        {
            sprintf(
                test,
                "%s %s",
                line,
                token
            );
        }

        int test_width =
            XTextWidth(
                font,
                test,
                strlen(test)
            );

        if (test_width < width)
        {
            if (strlen(line) == 0)
            {
                strcpy(line, token);
            }
            else
            {
                strcat(line, " ");
                strcat(line, token);
            }

            strcpy(
                words[word_count],
                token
            );

            word_count++;
        }
        else
        {
            drawJustifiedLine(
                display,
                window,
                gc,
                font,

                words,
                word_count,

                x,
                y,
                width
            );

            y += LINE_HEIGHT;

            strcpy(line, token);

            word_count = 0;

            strcpy(
                words[word_count],
                token
            );

            word_count++;
        }

        token = strtok(NULL, " ");
    }

    /*
        Последняя строка абзаца
        НЕ растягивается
    */

    XDrawString(
        display,
        window,
        gc,
        x,
        y,
        line,
        strlen(line)
    );

    y += LINE_HEIGHT;

    return y;
}

/*
    =====================================
    MAIN
    =====================================
*/

// ИЗМЕНЕНИЕ: Добавили параметры argc и argv для чтения аргументов командной строки
int main(int argc, char *argv[])
{
    Display *display;
    Window window;
    GC gc;
    XEvent event;

    int current_width = WIDTH;

    /*
        =====================================
        ОБРАБОТКА АРГУМЕНТОВ И ЧТЕНИЕ ФАЙЛА
        =====================================
    */
    char paragraphs[MAX_PARAGRAPHS][MAX_PARAGRAPH_LEN];
    int paragraph_count = 0;

    // Имя файла по умолчанию
    char *filename = "input.txt";

    // Проверяем: если argc > 1, значит пользователь передал аргумент при запуске
    if (argc > 1)
    {
        filename = argv[1]; // argv[0] — это имя самой программы, а argv[1] — первый аргумент
        printf("Открываем указанный файл: %s\n", filename);
    }
    else
    {
        printf("Аргумент не передан. Используем файл по умолчанию: %s\n", filename);
    }

    // Открываем файл (теперь имя файла берется из переменной filename)
    FILE *file = fopen(filename, "r");
    if (!file)
    {
        printf("Ошибка: Не удалось открыть файл \"%s\".\n", filename);
        // Подсказка пользователю, как правильно запускать программу
        printf("Инструкция по запуску: %s [имя_файла.txt]\n", argv[0]);
        return 1;
    }

    // Читаем файл построчно
    while (fgets(paragraphs[paragraph_count], MAX_PARAGRAPH_LEN, file))
    {
        // Удаляем символ переноса строки \n
        paragraphs[paragraph_count][strcspn(paragraphs[paragraph_count], "\n")] = 0;

        if (strlen(paragraphs[paragraph_count]) > 0)
        {
            paragraph_count++;
        }

        if (paragraph_count >= MAX_PARAGRAPHS)
        {
            break;
        }
    }
    fclose(file);

    /*
        =====================================
        ИНИЦИАЛИЗАЦИЯ XLIB
        =====================================
    */

    display = XOpenDisplay(NULL);
    if (!display)
    {
        printf("Ошибка подключения к X серверу\n");
        return 1;
    }

    int screen = DefaultScreen(display);

    window = XCreateSimpleWindow(
        display,
        RootWindow(display, screen),
        100, 100, WIDTH, HEIGHT, 1,
        BlackPixel(display, screen),
        WhitePixel(display, screen)
    );

    XStoreName(display, window, "Лаба 4 - Верстка текста из файла");

    XSelectInput(
        display, window,
        ExposureMask | StructureNotifyMask | KeyPressMask
    );

    gc = XCreateGC(display, window, 0, NULL);
    XMapWindow(display, window);

    /*
        =====================================
        ГЛАВНЫЙ ЦИКЛ
        =====================================
    */

    while (1)
    {
        XNextEvent(display, &event);

        if (event.type == ConfigureNotify)
        {
            current_width = event.xconfigure.width;
        }

        if (event.type == Expose || event.type == ConfigureNotify)
        {
            XClearWindow(display, window);

            XFontStruct *font = XQueryFont(display, XGContextFromGC(gc));

            int y = TOP_MARGIN;
            int text_width = current_width - LEFT_MARGIN * 2;

            for (int i = 0; i < paragraph_count; i++)
            {
                y = drawParagraph(
                    display, window, gc, font,
                    paragraphs[i],
                    LEFT_MARGIN, y, text_width
                );

                y += LINE_HEIGHT;
            }
        }

        if (event.type == KeyPress)
        {
            break;
        }
    }

    XFreeGC(display, gc);
    XCloseDisplay(display);

    return 0;
}
