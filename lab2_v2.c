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
    unsigned long color; // Хранит цвет (черный или белый для ластика)
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
        !XStringListToTextProperty(&ptrTitle, 1, &windowname) ||
        !XStringListToTextProperty(&ptrITitle, 1, &iconname)
    )
    {
        puts("No memory!");
        exit(1);
    }

    size_hints.flags = PPosition | PSize | PMinSize;
    size_hints.min_width = win_wdt_min;
    size_hints.min_height = win_hgt_min;

    wm_hints.flags = StateHint | InputHint;
    wm_hints.initial_state = NormalState;
    wm_hints.input = True;

    class_hint.res_name = argv[0];
    class_hint.res_class = PClass;

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

int main(int argc, char *argv[])
{
    Display *display;
    int screen_number;
    Window window;
    GC gc;
    XEvent report;

    Cpoint *points;
    int size = 0;

    int drawing = 0;
    int x0, y0;

    // Переменные для цвета и размеров окна
    unsigned long current_color;
    int win_width = WIDTH;
    int win_height = HEIGHT;

    points = (Cpoint *)malloc(sizeof(Cpoint));

    /*
        ==========================================
        ПОДКЛЮЧЕНИЕ К X11
        ==========================================
    */

    if ((display = XOpenDisplay(NULL)) == NULL)
    {
        puts("Can not connect to X server!");
        exit(1);
    }

    screen_number = DefaultScreen(display);

    /*
        ==========================================
        СОЗДАНИЕ ОКНА
        ==========================================
    */

    window = XCreateSimpleWindow(
        display,
        RootWindow(display, screen_number),
        X_POS, Y_POS,
        WIDTH, HEIGHT,
        BORDER_WIDTH,
        BlackPixel(display, screen_number),
        WhitePixel(display, screen_number)
    );

    SetWindowManagerHints(
        display, PRG_CLASS, argv, argc, window,
        X_POS, Y_POS, WIDTH, HEIGHT, WIDTH_MIN, HEIGHT_MIN,
        TITLE, ICON_TITLE, 0
    );

    /*
        ==========================================
        СОБЫТИЯ
        ==========================================
    */

    XSelectInput(
        display, window,
        ExposureMask | ButtonPressMask | ButtonReleaseMask |
        ButtonMotionMask | StructureNotifyMask | KeyPressMask
    );

    XMapWindow(display, window);

    /*
        ==========================================
        СОЗДАЁМ GC
        ==========================================
    */

    gc = XCreateGC(display, window, 0, NULL);
    XSetForeground(display, gc, BlackPixel(display, screen_number));

    /*
        ==========================================
        ГЛАВНЫЙ ЦИКЛ
        ==========================================
    */

    while (1)
    {
        XNextEvent(display, &report);

        switch (report.type)
        {
            /*
                ==================================
                ПЕРЕРИСОВКА
                ==================================
            */
            case Expose:
                if (report.xexpose.count != 0) break;

                for (int i = 0; i < size - 1; i++)
                {
                    if (points[i].flag != end)
                    {
                        XSetForeground(display, gc, points[i].color);
                        XDrawLine(
                            display, window, gc,
                            points[i].x, points[i].y,
                            points[i + 1].x, points[i + 1].y
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
                // ЛКМ - карандаш, ПКМ - ластик
                if (report.xbutton.button == Button1)
                {
                    current_color = BlackPixel(display, screen_number);
                    drawing = 1;
                }
                else if (report.xbutton.button == Button3)
                {
                    current_color = WhitePixel(display, screen_number);
                    drawing = 1;
                }
                else
                {
                    drawing = 0;
                    break;
                }

                size++;
                points = (Cpoint *)realloc(points, size * sizeof(Cpoint));

                x0 = report.xbutton.x;
                y0 = report.xbutton.y;

                // Ограничения
                if (x0 < 0) x0 = 0;
                if (x0 > win_width) x0 = win_width;
                if (y0 < 0) y0 = 0;
                if (y0 > win_height) y0 = win_height;

                points[size - 1].x = x0;
                points[size - 1].y = y0;
                points[size - 1].flag = begin;
                points[size - 1].color = current_color;
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
                    points[size - 1].flag = end;
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
                    int cur_x = report.xmotion.x;
                    int cur_y = report.xmotion.y;

                    // Ограничения, чтобы не рисовать за окном
                    if (cur_x < 0) cur_x = 0;
                    if (cur_x > win_width) cur_x = win_width;
                    if (cur_y < 0) cur_y = 0;
                    if (cur_y > win_height) cur_y = win_height;

                    XSetForeground(display, gc, current_color);
                    XDrawLine(display, window, gc, x0, y0, cur_x, cur_y);

                    x0 = cur_x;
                    y0 = cur_y;

                    size++;
                    points = (Cpoint *)realloc(points, size * sizeof(Cpoint));

                    points[size - 1].x = x0;
                    points[size - 1].y = y0;
                    points[size - 1].flag = line;
                    points[size - 1].color = current_color;
                }
                break;

            /*
                ==================================
                ИЗМЕНЕНИЕ РАЗМЕРА ОКНА
                ==================================
            */
            case ConfigureNotify:
                win_width = report.xconfigure.width;
                win_height = report.xconfigure.height;

                XClearWindow(display, window);

                for (int i = 0; i < size - 1; i++)
                {
                    if (points[i].flag != end)
                    {
                        XSetForeground(display, gc, points[i].color);
                        XDrawLine(
                            display, window, gc,
                            points[i].x, points[i].y,
                            points[i + 1].x, points[i + 1].y
                        );
                    }
                }
                break;
        }
    }

    free(points);
    XFreeGC(display, gc);
    XCloseDisplay(display);

    return 0;
}
