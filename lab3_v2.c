#include <X11/Xlib.h>
#include <X11/Xutil.h>
#include <X11/Xos.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#define X 0
#define Y 0
#define WIDTH 800
#define HEIGHT 600
#define WIDTH_MIN 50
#define HEIGHT_MIN 50
#define BORDER_WIDTH 5
#define TITLE "XPaint"
#define ICON_TITLE "XPaint"
#define PRG_CLASS "XPaint"

enum status {
    begin,
    line,
    end
};

struct Cpoint {
    int x, y;
    unsigned long color;
    enum status flag;
};

typedef struct Cpoint Cpoint;

static void SetWindowManagerHints (
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

    if (!XStringListToTextProperty(&ptrTitle, 1, &windowname) ||
        !XStringListToTextProperty(&ptrITitle, 1, &iconname)) {
        puts("No memory!\n");
        exit(1);
    }

    size_hints.flags = PPosition | PSize | PMinSize;
    size_hints.min_width = win_wdt_min;
    size_hints.min_height = win_hgt_min;

    wm_hints.flags = StateHint | IconPixmapHint | InputHint;
    wm_hints.initial_state = NormalState;
    wm_hints.input = True;
    wm_hints.icon_pixmap = pixmap;

    class_hint.res_name = argv[0];
    class_hint.res_class = PClass;

    XSetWMProperties(display, window, &windowname,
                     &iconname, argv, argc, &size_hints, &wm_hints,
                     &class_hint);
}

void create_color_window(Display *d, Window rootwindow, Window *w, int screen_number, int *x, int y, int color) {
    int size = 20, distance = 7;
    int border_widght = 2;
    *w = XCreateSimpleWindow(d,
                             rootwindow,
                             *x, y, size, size, border_widght,
                             BlackPixel(d, screen_number),
                             WhitePixel(d, screen_number));
    XSetWindowBackground(d, *w, color);
    XSelectInput(d, *w, ExposureMask | ButtonPressMask);
    *x += size + distance;
}

void set_pero_color(Display *display, GC gc, Window color_window[], int color[], int index, int *selected_window) {
    XSetWindowBorder(display, color_window[index], 0xFFD700); 
    XSetWindowBorder(display, color_window[*selected_window], 0x808080); 
    *selected_window = index;
}

int main(int argc, char *argv[])
{
    Display *display;
    int screen_number;
    GC gc;
    XEvent report;
    Window window, color_window[4];
    Cpoint *points;

    int color[4] = {0x000000, 0xFF0000, 0x008000, 0x0000FF};
    int selected_window = 0;
    int left_x = 2;
    int flag = 0, x0, y0, size = 0;

    unsigned long current_draw_color;
    int win_width = WIDTH;
    int win_height = HEIGHT;

    points = (Cpoint *)malloc(sizeof(Cpoint));

    if ((display = XOpenDisplay(NULL)) == NULL) {
        puts("Can not connect to the X server!\n");
        exit(1);
    }
    screen_number = DefaultScreen(display);

    window = XCreateSimpleWindow(display,
                                 RootWindow(display, screen_number),
                                 X, Y, WIDTH, HEIGHT, BORDER_WIDTH,
                                 BlackPixel(display, screen_number),
                                 WhitePixel(display, screen_number));

    SetWindowManagerHints(display, PRG_CLASS, argv, argc,
                          window, X, Y, WIDTH, HEIGHT, WIDTH_MIN,
                          HEIGHT_MIN, TITLE, ICON_TITLE, 0);

    XSelectInput(display, window, ExposureMask | KeyPressMask | ButtonPressMask | ButtonReleaseMask | ButtonMotionMask | StructureNotifyMask);
    XMapWindow(display, window);

    for(int i = 0; i < 4; ++i) {
        create_color_window(display, window, &color_window[i], screen_number, &left_x, 5, color[i]);
        XMapWindow(display, color_window[i]);
    }

    XSetWindowBorder(display, color_window[0], 0xFFD700);
    for (int i = 1; i < 4; ++i)
        XSetWindowBorder(display, color_window[i], 0x808080);

    gc = XCreateGC(display, window, 0, NULL);
    XSetForeground(display, gc, color[selected_window]);

    while (1) {
        XNextEvent(display, &report);

        switch (report.type) {
            case Expose:
                if (report.xexpose.count != 0) break;

                for (int i = 0; i < size - 1; ++i) {
                    if (points[i].flag == begin) {
                        XSetForeground(display, gc, points[i].color);
                    }
                    if (points[i].flag != end) {
                        XDrawLine(display, window, gc, points[i].x, points[i].y, points[i+1].x, points[i+1].y);
                    }
                }
                break;

            case ButtonPress:
                if (report.xbutton.window != window) {
                    if (report.xbutton.window == color_window[0])
                        set_pero_color(display, gc, color_window, color, 0, &selected_window);
                    else if (report.xbutton.window == color_window[1])
                        set_pero_color(display, gc, color_window, color, 1, &selected_window);
                    else if (report.xbutton.window == color_window[2])
                        set_pero_color(display, gc, color_window, color, 2, &selected_window);
                    else if (report.xbutton.window == color_window[3])
                        set_pero_color(display, gc, color_window, color, 3, &selected_window);
                }
                else {
                    if (report.xbutton.button == Button1) {
                        current_draw_color = color[selected_window];
                        flag = 1;

                        size++;
                        points = (Cpoint*)realloc(points, size * sizeof(Cpoint));

                        x0 = report.xbutton.x;
                        y0 = report.xbutton.y;

                        points[size-1].x = x0;
                        points[size-1].y = y0;
                        points[size-1].color = current_draw_color;
                        points[size-1].flag = begin;
                    } 
                    else if (report.xbutton.button == Button3) {
                        XClearWindow(display, window); 
                        size = 0;                      
                        flag = 0;                      
                    } 
                    else {
                        flag = 0; 
                    }
                }
                break;

            case ButtonRelease:
                if (flag) {
                    flag = 0;
                    if (size > 0) points[size-1].flag = end;
                }
                break;

            case MotionNotify:
                if (flag) {
                    int cur_x = report.xmotion.x;
                    int cur_y = report.xmotion.y;

                    XSetForeground(display, gc, current_draw_color);
                    XDrawLine(display, window, gc, x0, y0, cur_x, cur_y);

                    size++;
                    points = (Cpoint*)realloc(points, size * sizeof(Cpoint));

                    x0 = cur_x;
                    y0 = cur_y;

                    points[size-1].x = x0;
                    points[size-1].y = y0;
                    points[size-1].color = current_draw_color;
                    points[size-1].flag = line;
                }
                break;

            case ConfigureNotify:
                win_width = report.xconfigure.width;
                win_height = report.xconfigure.height;

                XClearWindow(display, window);

                for(int i = 0; i < 4; ++i) {
                    XMapWindow(display, color_window[i]);
                }

                for (int i = 0; i < size - 1; ++i) {
                    if (points[i].flag == begin) {
                        XSetForeground(display, gc, points[i].color);
                    }
                    if (points[i].flag != end) {
                        XDrawLine(display, window, gc, points[i].x, points[i].y, points[i+1].x, points[i+1].y);
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
