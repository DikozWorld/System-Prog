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

int main()
{
    Display *display;

    Window window;

    GC gc;

    XEvent event;

    int current_width =
        WIDTH;

    /*
        Исходный текст
    */

    char *paragraphs[] =
    {
        "Systems programming studies the operating principles of operating systems and software that interact with hardware. One of the primary tools for graphical programming in Linux is the Xlib library.",

        "The Xlib library provides a low-level interface for working with the X11 windowing system. It can be used to create windows, handle keyboard and mouse events, and perform graphical output.",

        "When the window size changes, the text should automatically reflow. Paragraphs should be preserved and not blended."
    };

    int paragraph_count = 3;

    display = XOpenDisplay(NULL);

    if (!display)
    {
        printf("Ошибка подключения\n");
        return 1;
    }

    int screen =
        DefaultScreen(display);

    window =
        XCreateSimpleWindow(
            display,
            RootWindow(
                display,
                screen
            ),

            100,
            100,

            WIDTH,
            HEIGHT,

            1,

            BlackPixel(
                display,
                screen
            ),

            WhitePixel(
                display,
                screen
            )
        );

    XStoreName(
        display,
        window,
        "Лаба 4 - Верстка текста"
    );

    XSelectInput(
        display,
        window,

        ExposureMask |
        StructureNotifyMask |
        KeyPressMask
    );

    gc =
        XCreateGC(
            display,
            window,
            0,
            NULL
        );

    XMapWindow(
        display,
        window
    );

    while (1)
    {
        XNextEvent(
            display,
            &event
        );

        if (
            event.type ==
            ConfigureNotify
        )
        {
            current_width =
                event.xconfigure.width;
        }

        if (
            event.type ==
            Expose
            ||
            event.type ==
            ConfigureNotify
        )
        {
            XClearWindow(
                display,
                window
            );

            XFontStruct *font =
                XQueryFont(
                    display,
                    XGContextFromGC(gc)
                );

            int y =
                TOP_MARGIN;

            int text_width =
                current_width -
                LEFT_MARGIN * 2;

            for (
                int i = 0;
                i < paragraph_count;
                i++
            )
            {
                y =
                    drawParagraph(
                        display,
                        window,
                        gc,
                        font,

                        paragraphs[i],

                        LEFT_MARGIN,
                        y,

                        text_width
                    );

                /*
                    Пустая строка
                    между абзацами
                */

                y += LINE_HEIGHT;
            }
        }

        if (
            event.type ==
            KeyPress
        )
        {
            break;
        }
    }

    XFreeGC(
        display,
        gc
    );

    XCloseDisplay(
        display
    );

    return 0;
}
