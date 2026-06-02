#include <X11/Xlib.h>
#include <X11/Xutil.h>
#include <X11/keysym.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <math.h>
#include <sys/time.h>
#include <unistd.h>

// Макросы координат и окна
#define X_POS 0
#define Y_POS 0
#define WIDTH 1200
#define HEIGHT 800
#define WIDTH_MIN 50
#define HEIGHT_MIN 50
#define BORDER_WIDTH 5

#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

Display *display;
GC gc;
Window window;
Pixmap double_buffer;
int screen_number;

int game_started = 0;
int game_over = 0;
double elapsed_time = 0.0;

struct {
    int x, y, w, h;
    char text[10];
} start_btn;

// Хищник (Черная птица)
struct {
    double x, y;
    double size;
    int dir_x, dir_y;
    double speed; // пикселей в секунду
} hero;


struct {
    double x, y;
    double size;
    double angle;       // Текущий угол (градусы)
    double angular_spd; // Градусов в секунду (10 град / 0.05 сек = 200 град/сек)
    double total_dist;  // Суммарный угол для ускорения
    double radius;      // Радиус полета
} enemy;

// Получение текущего времени в секундах
double get_time_sec() {
    struct timeval tv;
    gettimeofday(&tv, NULL);
    return (double)tv.tv_sec + (double)tv.tv_usec / 1000000.0;
}

// Отрисовка всего кадра в буфер
void render_frame() {
    // 1. Очищаем фон (заливаем белым)
    XSetForeground(display, gc, WhitePixel(display, screen_number));
    XFillRectangle(display, double_buffer, gc, 0, 0, WIDTH, HEIGHT);

    if (!game_started && !game_over) {
        // Рисуем кнопку
        XSetForeground(display, gc, BlackPixel(display, screen_number));
        XFillRectangle(display, double_buffer, gc, start_btn.x, start_btn.y, start_btn.w, start_btn.h);
        XSetForeground(display, gc, WhitePixel(display, screen_number));
        XDrawString(display, double_buffer, gc, start_btn.x + 25, start_btn.y + 25, start_btn.text, strlen(start_btn.text));
    }
    else if (game_over) {
        char msg[128];
        sprintf(msg, "Game over, time of game: %.2f sec.", elapsed_time);
        XSetForeground(display, gc, BlackPixel(display, screen_number));
        XDrawString(display, double_buffer, gc, WIDTH / 2 - 100, HEIGHT / 2, msg, strlen(msg));
    }
    else {
        // Рисуем Черную птицу (квадрат)
        XSetForeground(display, gc, 0x000000);
        XFillRectangle(display, double_buffer, gc, (int)hero.x, (int)hero.y, (int)hero.size, (int)hero.size);

        // Рисуем Синюю птицу (круг)
        XSetForeground(display, gc, 0x0000FF);
        XFillArc(display, double_buffer, gc, (int)enemy.x, (int)enemy.y, (int)enemy.size, (int)enemy.size, 0, 360 * 64);
    }

    // Копируем готовый кадр из буфера на экран (мгновенно, без мерцания)
    XCopyArea(display, double_buffer, window, gc, 0, 0, WIDTH, HEIGHT, 0, 0);
    XFlush(display);
}

// Проверка поимки с точностью 10х10 (ТЗ №3)
int check_capture() {
    double cx_hero = hero.x + hero.size / 2.0;
    double cy_hero = hero.y + hero.size / 2.0;
    double cx_enemy = enemy.x + enemy.size / 2.0;
    double cy_enemy = enemy.y + enemy.size / 2.0;

    if (fabs(cx_hero - cx_enemy) <= 10.0 && fabs(cy_hero - cy_enemy) <= 10.0) {
        return 1;
    }
    return 0;
}

int main(int argc, char *argv[]) {
    if ((display = XOpenDisplay(NULL)) == NULL) {
        puts("Can not connect to the X server!\n");
        exit(1);
    }
    screen_number = DefaultScreen(display);

    // Настройка стартовых параметров
    start_btn.w = 80; start_btn.h = 40;
    start_btn.x = (WIDTH - start_btn.w) / 2;
    start_btn.y = (HEIGHT - start_btn.h) / 2;
    strcpy(start_btn.text, "Пуск");

    hero.x = 50; hero.y = 50; hero.size = 30;
    hero.dir_x = 0; hero.dir_y = 0; hero.speed = 600.0; // Очень быстрая, чтобы поймать жертву

    enemy.size = 20;
    enemy.angle = 0.0;
    enemy.radius = 150.0;
    enemy.angular_spd = 200.0; // 10 градусов за 0.05 сек = 200 град в секунду
    enemy.total_dist = 0.0;

    // Создание окна
    window = XCreateSimpleWindow(display, RootWindow(display, screen_number),
                                 X_POS, Y_POS, WIDTH, HEIGHT, BORDER_WIDTH,
                                 BlackPixel(display, screen_number),
                                 WhitePixel(display, screen_number));

    // Создаем буфер для плавного рисования
    double_buffer = XCreatePixmap(display, window, WIDTH, HEIGHT, DefaultDepth(display, screen_number));
    gc = XCreateGC(display, window, 0, NULL);

    XSelectInput(display, window, ExposureMask | KeyPressMask | ButtonPressMask);
    XMapWindow(display, window);

    // Игровой цикл
    double last_time = get_time_sec();
    XEvent report;
    KeySym key;

    while (1) {
        double current_time = get_time_sec();
        double dt = current_time - last_time; // дельта времени в секундах (обычно ~0.016 для 60FPS)
        last_time = current_time;

        // 1. Обработка всех накопившихся событий (без зависаний)
        while (XPending(display)) {
            XNextEvent(display, &report);
            switch (report.type) {
                case Expose:
                    render_frame();
                    break;
                case ButtonPress:
                    if (!game_started && !game_over && report.xbutton.button == Button1) {
                        if (report.xbutton.x >= start_btn.x && report.xbutton.x <= start_btn.x + start_btn.w &&
                            report.xbutton.y >= start_btn.y && report.xbutton.y <= start_btn.y + start_btn.h) {
                            game_started = 1;
                        }
                    }
                    break;
                case KeyPress:
                    if (!game_started || game_over) break;
                    key = XLookupKeysym(&report.xkey, 0);
                    if (key == XK_Left)       { hero.dir_x = -1; hero.dir_y = 0; }
                    else if (key == XK_Right) { hero.dir_x = 1;  hero.dir_y = 0; }
                    else if (key == XK_Up)    { hero.dir_x = 0;  hero.dir_y = -1; }
                    else if (key == XK_Down)  { hero.dir_x = 0;  hero.dir_y = 1; }
                    else if (key == XK_Escape) { game_over = 1; }
                    break;
            }
        }

        // 2. Физика (только если игра идет)
        if (game_started && !game_over) {
            elapsed_time += dt;

            // --- Хищник ---
            hero.x += hero.dir_x * hero.speed * dt;
            hero.y += hero.dir_y * hero.speed * dt;

            // Остановка об края (до упора)
            if (hero.x < 0) { hero.x = 0; hero.dir_x = 0; }
            if (hero.x > WIDTH - hero.size) { hero.x = WIDTH - hero.size; hero.dir_x = 0; }
            if (hero.y < 0) { hero.y = 0; hero.dir_y = 0; }
            if (hero.y > HEIGHT - hero.size) { hero.y = HEIGHT - hero.size; hero.dir_y = 0; }

            // --- Жертва ---
            double delta_angle = enemy.angular_spd * dt;
            enemy.angle += delta_angle;
            enemy.total_dist += delta_angle;

            // Ускорение: 1 градус за 0.05с (то есть +20 град/сек) на каждые 360 градусов
            if (enemy.total_dist >= 360.0) {
                enemy.angular_spd += 20.0;
                enemy.total_dist -= 360.0;
            }

            // Центр окружности (колебание за 18 секунд)
            // Амплитуда = W/2, но вычитаем радиус, чтобы круг не улетал за края экрана
            double amplitude = (WIDTH / 2.0) - enemy.radius - 20;
            double center_x = (WIDTH / 2.0) + amplitude * sin((2.0 * M_PI * elapsed_time) / 18.0);
            double center_y = HEIGHT / 2.0;

            enemy.x = center_x + enemy.radius * cos(enemy.angle * M_PI / 180.0) - (enemy.size / 2.0);
            enemy.y = center_y + enemy.radius * sin(enemy.angle * M_PI / 180.0) - (enemy.size / 2.0);

            // --- Поимка ---
            if (check_capture()) {
                game_over = 1;
            }
        }

        // 3. Отрисовка
        render_frame();

        // 4. Пауза 16 миллисекунд (~60 FPS), чтобы не грузить процессор
        usleep(16000);

        if (game_over && key == XK_Escape) break; // Выход
    }

    XFreePixmap(display, double_buffer);
    XFreeGC(display, gc);
    XDestroyWindow(display, window);
    XCloseDisplay(display);
    return 0;
}
