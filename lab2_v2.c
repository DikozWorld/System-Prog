#include <X11/Xlib.h>
#include <X11/Xutil.h>
#include <X11/Xos.h>

#include <stdio.h>
#include <stdlib.h>

#define X_POS 0
#define Y_POS 0

#define WIDTH 800
#define HEIGHT 600

#define WIDTH_MIN 50
#define HEIGHT_MIN 50

#define BORDER_WIDTH 5

#define TITLE "Paint 1 Color"
#define ICON_TITLE "Paint 1 Color"
#define PRG_CLASS "Paint"

/*
    ==========================================
    ТИП ТОЧКИ
    ==========================================
*/

enum status
{
    begin,
    line,
    end
};

struct Cpoint
{
    int x;
    int y;

    enum status flag;
};

typedef struct Cpoint Cpoint;

/*
    ==========================================
    НАСТРОЙКА ОКНА
    ==========================================
*/

static void SetWindowManagerHints(
    Display *display,
    char *PClass,
    char *argv[],
    int argc,
    Window window,
    int x,
    int y,
    int win_wdt,
    int win_hgt,
    int win_wdt_min,
    int win_hgt_min,
    char *ptrTitle,
    char *ptrITitle,
    Pixmap pixmap
)
{
    XSizeHints size_hints;

    XWMHints wm_hints;
    XClassHint class_hint;
    XTextProperty windowname, iconname;

    if (
        !XStringListToTextProperty(
            &ptrTitle,
            1,
            &windowname
        )
        ||
        !XStringListToTextProperty(
            &ptrITitle,
            1,
            &iconname
        )
    )
    {
        puts("No memory!");
        exit(1);
    }

    size_hints.flags =
        PPosition |
        PSize |
        PMinSize;

    size_hints.min_width =
        win_wdt_min;

    size_hints.min_height =
        win_hgt_min;

    wm_hints.flags =
        StateHint |
        InputHint;

    wm_hints.initial_state =
        NormalState;

    wm_hints.input = True;

    class_hint.res_name =
        argv[0];

    class_hint.res_class =
        PClass;

    XSetWMProperties(
        display,
        window,
        &windowname,
        &iconname,
        argv,
        argc,
        &size_hints,
        &wm_hints,
        &class_hint
    );
}

/*
    ==========================================
    MAIN
    ==========================================
*/

int main(
    int argc,
    char *argv[]
)
{
    Display *display;

    int screen_number;

    Window window;

    GC gc;

    XEvent report;

    /*
        МАССИВ ТОЧЕК
    */

    Cpoint *points;

    int size = 0;

    /*
        РЕЖИМ РИСОВАНИЯ
    */

    int drawing = 0;

    /*
        ПРЕДЫДУЩАЯ ТОЧКА
    */

    int x0;
    int y0;

    points =
        (Cpoint *)malloc(
            sizeof(Cpoint)
        );

    /*
        ==========================================
        ПОДКЛЮЧЕНИЕ К X11
        ==========================================
    */

    if (
        (display =
            XOpenDisplay(NULL)
        ) == NULL
    )
    {
        puts(
            "Can not connect to X server!"
        );

        exit(1);
    }

    screen_number =
        DefaultScreen(display);

    /*
        ==========================================
        СОЗДАНИЕ ОКНА
        ==========================================
    */

    window =
        XCreateSimpleWindow(

            display,

            RootWindow(
                display,
                screen_number
            ),

            X_POS,
            Y_POS,

            WIDTH,
            HEIGHT,

            BORDER_WIDTH,

            BlackPixel(
                display,
                screen_number
            ),

            WhitePixel(
                display,
                screen_number
            )
        );

    SetWindowManagerHints(

        display,

        PRG_CLASS,

        argv,

        argc,

        window,

        X_POS,
        Y_POS,

        WIDTH,
        HEIGHT,

        WIDTH_MIN,
        HEIGHT_MIN,

        TITLE,
        ICON_TITLE,

        0
    );

    /*
        ==========================================
        СОБЫТИЯ
        ==========================================
    */

    XSelectInput(

        display,

        window,

        ExposureMask |
        ButtonPressMask |
        ButtonReleaseMask |
        ButtonMotionMask |
        StructureNotifyMask |
        KeyPressMask
    );

    XMapWindow(
        display,
        window
    );

    /*
        ==========================================
        СОЗДАЁМ GC
        ==========================================
    */

    gc =
        XCreateGC(
            display,
            window,
            0,
            NULL
        );

    XSetForeground(
        display,
        gc,
        BlackPixel(
            display,
            screen_number
        )
    );

    /*
        ==========================================
        ГЛАВНЫЙ ЦИКЛ
        ==========================================
    */

    while (1)
    {
        XNextEvent(
            display,
            &report
        );

        switch (report.type)
        {
            /*
                ==================================
                ПЕРЕРИСОВКА
                ==================================
            */

            case Expose:

                if (
                    report.xexpose.count != 0
                )
                {
                    break;
                }

                for (
                    int i = 0;
                    i < size - 1;
                    i++
                )
                {
                    if (
                        points[i].flag != end
                    )
                    {
                        XDrawLine(

                            display,
                            window,
                            gc,

                            points[i].x,
                            points[i].y,

                            points[i + 1].x,
                            points[i + 1].y
                        );
                    }
                }

                break;

            /*
                ==================================
                НАЖАТИЕ МЫШИ
                ==================================
            */

            case ButtonPress:

                drawing = 1;

                size++;

                points =
                    (Cpoint *)realloc(
                        points,
                        size *
                        sizeof(Cpoint)
                    );

                x0 =
                    report.xbutton.x;

                y0 =
                    report.xbutton.y;

                points[size - 1].x =
                    x0;

                points[size - 1].y =
                    y0;

                points[size - 1].flag =
                    begin;

                break;

            /*
                ==================================
                ОТПУСКАНИЕ МЫШИ
                ==================================
            */

            case ButtonRelease:

                drawing = 0;

                if (size > 0)
                {
                    points[size - 1].flag =
                        end;
                }

                break;

            /*
                ==================================
                ДВИЖЕНИЕ МЫШИ
                ==================================
            */

            case MotionNotify:

                if (drawing)
                {
                    XDrawLine(

                        display,
                        window,
                        gc,

                        x0,
                        y0,

                        report.xmotion.x,
                        report.xmotion.y
                    );

                    x0 =
                        report.xmotion.x;

                    y0 =
                        report.xmotion.y;

                    size++;

                    points =
                        (Cpoint *)realloc(
                            points,
                            size *
                            sizeof(Cpoint)
                        );

                    points[size - 1].x =
                        x0;

                    points[size - 1].y =
                        y0;

                    points[size - 1].flag =
                        line;
                }

                break;

            /*
                ==================================
                ИЗМЕНЕНИЕ РАЗМЕРА ОКНА
                ==================================
            */

            case ConfigureNotify:

                XClearWindow(
                    display,
                    window
                );

                for (
                    int i = 0;
                    i < size - 1;
                    i++
                )
                {
                    if (
                        points[i].flag != end
                    )
                    {
                        XDrawLine(

                            display,
                            window,
                            gc,

                            points[i].x,
                            points[i].y,

                            points[i + 1].x,
                            points[i + 1].y
                        );
                    }
                }

                break;
        }
    }

    free(points);

    XFreeGC(
        display,
        gc
    );

    XCloseDisplay(
        display
    );

    return 0;
}
