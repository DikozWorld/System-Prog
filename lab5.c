#include <X11/Xlib.h>
#include <X11/Xutil.h>
#include <X11/Xos.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <pthread.h>
#include <math.h>
#include <X11/keysymdef.h>
#define X 0
#define Y 0
#define WIDTH 1200
#define HEIGHT 800
#define WIDTH_MIN 50
#define HEIGHT_MIN 50
#define BORDER_WIDTH 5
#define TITLE "Arcadegame"
#define ICON_TITLE "Arcadegame"
#define PRG_CLASS "Arcadegame"

double speed =1L;
Display * display;
GC gc;
Window window;
int screen_number, timer=0;
enum status{
    hero,
    enemy
};
struct Object {
    int color=0x000000;
    int speed_x=10,speed_y=10;
    status flag=hero;
    double x=10,y=10,size=20;
    
    
};

int cheker (Object hero[2]){
    for (int i=hero[0].y;i<hero[0].y+hero[0].size;++i)
        if (pow((hero[0].x-hero[1].x-hero[1].size/2)*(hero[0].x-hero[1].x-hero[1].size/2) + (i-hero[1].y-hero[1].size/2)*(i-hero[1].y-hero[1].size/2),0.5)<hero[1].size/2)
            return 0;
    for (int i=hero[0].y;i<hero[0].y+hero[0].size;++i)
        if (pow((hero[0].x+hero[0].size-hero[1].x-hero[1].size/2)*(hero[0].x+hero[0].size-hero[1].x-hero[1].size/2) + (i-hero[1].y-hero[1].size/2)*(i-hero[1].y-hero[1].size/2),0.5)<hero[1].size/2)
            return 0;
    for (int i=hero[0].x;i<hero[0].x+hero[0].size;++i)
        if (pow((i-hero[1].x-hero[1].size/2)*(i-hero[1].x-hero[1].size/2) + (hero[0].y-hero[1].y-hero[1].size/2)*(hero[0].y-hero[1].y-hero[1].size/2),0.5)<hero[1].size/2)
            return 0;
    for (int i=hero[0].x;i<hero[0].x+hero[0].size;++i)
        if (pow((i-hero[1].x-hero[1].size/2)*(i-hero[1].x-hero[1].size/2) + (hero[0].y+hero[0].size-hero[1].y-hero[1].size/2)*(hero[0].y+hero[0].size-hero[1].y-hero[1].size/2),0.5)<hero[1].size/2)
            return 0;
    return 1;
    
}

typedef struct Object Object;

void* repaint(void * objects){
    Object * hero = (Object *) objects;
    XEvent e;
    while(1){
        
        XSetForeground ( display, gc,  0xFFFFFF);
        XFillArc(display,window,gc, hero[1].x,hero[1].y,hero[1].size,hero[1].size,0,360*64);
        do{
            hero[1].speed_x=rand()%4 *50 -4*50/2;
            
        }while(hero[1].x+hero[1].speed_x-hero[1].size<0 || hero[1].x+hero[1].speed_x+hero[1].size>WIDTH-hero[1].size);
         do{
            hero[1].speed_y=rand()%4 *50 -4*50/2;
            
        }while(hero[1].y+hero[1].speed_y-hero[1].size<0 || hero[1].y+hero[1].speed_y+hero[1].size>HEIGHT-hero[1].size);
        hero[1].x+=hero[1].speed_x;
        hero[1].y+=hero[1].speed_y;
        XSetForeground ( display, gc,  hero[1].color);
        XFillArc(display,window,gc, hero[1].x,hero[1].y,hero[1].size,hero[1].size,0,360*64);
        memset(&e, 0, sizeof(e));
        e.type=Expose;
        XSendEvent(display, window, false, Expose, &e);
        XFlush(display);
        timer+=1;
        sleep(speed);
        
    }
Window totalscore;
GC gct; 
char seconds[64],msg[64]="Total time: ";
sprintf(seconds,"%d",timer);
strncat (msg,seconds,strlen(msg));
strncat (msg," seconds.",strlen(msg));
totalscore=XCreateSimpleWindow ( display,RootWindow ( display, screen_number ),WIDTH/2, HEIGHT/2, 500, 300, BORDER_WIDTH,BlackPixel ( display, screen_number ),WhitePixel ( display, screen_number ) );
XSelectInput ( display, totalscore, ButtonPressMask | ButtonReleaseMask);
XMapWindow ( display, totalscore );
gct= XCreateGC ( display, totalscore, 0 , NULL );
XDrawString(display, totalscore, gct, 500/3-strlen(msg), 300/2, msg, strlen(msg));
printf("%s",msg);

}
void herorepaint(Display * display,GC gc,Window window, Object hero,int color){
    XSetForeground ( display, gc,  color);
    XFillRectangle(display,window,gc, hero.x,hero.y,hero.size,hero.size);
}
static void SetWindowManagerHints (
 Display * display,
 char * PClass,
 char * argv[],
 int argc,
 Window window,
 int x,
 int y,
 int win_wdt,
 int win_hgt,
 int win_wdt_min,
 int win_hgt_min,
 char * ptrTitle,
 char * ptrITitle,
 Pixmap pixmap
)
{
 XSizeHints size_hints;
 XWMHints wm_hints;
 XClassHint class_hint;
 XTextProperty windowname, iconname;

 if ( !XStringListToTextProperty (&ptrTitle, 1, &windowname ) ||
    !XStringListToTextProperty (&ptrITitle, 1, &iconname ) ) {
  puts ( "No memory!\n");
  exit ( 1 );
}

size_hints.flags = PPosition | PSize | PMinSize;
size_hints.min_width = win_wdt_min;
size_hints.min_height = win_hgt_min;
wm_hints.flags = StateHint | IconPixmapHint | InputHint;
wm_hints.initial_state = NormalState;
wm_hints.input = True;
wm_hints.icon_pixmap= pixmap;
class_hint.res_name = argv[0];
class_hint.res_class = PClass;

XSetWMProperties ( display, window, &windowname,
  &iconname, argv, argc, &size_hints, &wm_hints,
  &class_hint );
}



int main(int argc, char *argv[])
{
srand(time(NULL));
XEvent report;
Object heroes[2];
heroes[1].flag=enemy;
KeySym keycd;
char keystr[1];
int flag=1; 
GC gct;
 if ( ( display = XOpenDisplay ( NULL ) ) == NULL ) {
    puts ("Can not connect to the X server!\n");
    exit ( 1 );
 }

screen_number = DefaultScreen ( display );
window = XCreateSimpleWindow ( display,
                               RootWindow ( display, screen_number ),
                               X, Y, WIDTH, HEIGHT, BORDER_WIDTH,
                               BlackPixel ( display, screen_number ),
                               WhitePixel ( display, screen_number ) );
SetWindowManagerHints ( display, PRG_CLASS, argv, argc,
                        window, X, Y, WIDTH, HEIGHT, WIDTH_MIN,
                        HEIGHT_MIN, TITLE, ICON_TITLE, 0 );

XSelectInput ( display, window, ExposureMask | KeyPressMask);
XMapWindow ( display, window );
pthread_t enemy_thread;
for(int i=0; i<2;++i){
    if(heroes[i].flag==enemy){
        heroes[i].x=WIDTH-heroes[i].size-10;
        heroes[i].y=HEIGHT-heroes[i].size-10;
        heroes[i].color=0xFF0000;
    }
}
gc = XCreateGC ( display, window, 0 , NULL );
pthread_create(&enemy_thread, NULL, repaint, (void *)heroes);

flag=cheker(heroes);
 while (1) {
       for(int i=0;i<2;++i){
        XSetForeground ( display, gc,heroes[i].color);
        if(heroes[i].flag==hero)
            XFillRectangle(display,window,gc, heroes[i].x,heroes[i].y,heroes[i].size,heroes[i].size);
        else
            XFillArc(display,window,gc, heroes[i].x,heroes[i].y,heroes[i].size,heroes[i].size,0,360*64);}
     if(!flag){
        char seconds[64],msg[64]="Total time: ";
        sprintf(seconds,"%d",timer);
        strncat (msg,seconds,strlen(msg));
        strncat (msg," seconds.",strlen(msg));
        XDrawString(display, window, gc, WIDTH/3-strlen(msg), HEIGHT/2, msg, strlen(msg));
    }
    XNextEvent ( display, &report );

    switch ( report.type ) {
        case Expose :
            if ( report.xexpose.count != 0 )
                break;
            break;
        case KeyPress:
            memset(keystr,0,sizeof(keystr));
            XLookupString(&report.xkey,keystr,
                            sizeof (keystr), &keycd, NULL);
            if(*keystr=='w'){
                    if(heroes[0].y>0){
                        herorepaint(display,gc,window,heroes[0],0xFFFFFF);
                        heroes[0].y-=heroes[0].speed_y;
                        herorepaint(display,gc,window,heroes[0],heroes[0].color);
                    }
            }
               else if(*keystr=='s'){
                    if(heroes[0].y<HEIGHT-heroes[0].size){
                        herorepaint(display,gc,window,heroes[0],0xFFFFFF);
                        heroes[0].y+=heroes[0].speed_y;
                        herorepaint(display,gc,window,heroes[0],heroes[0].color);
                    }

                }


           else if(*keystr== 'a'){
                    if(heroes[0].x>0){
                        herorepaint(display,gc,window,heroes[0],0xFFFFFF);
                        heroes[0].x-=heroes[0].speed_x;
                        herorepaint(display,gc,window,heroes[0],heroes[0].color);
                    }
           }
             else if(*keystr=='d'){
                    if(heroes[0].x<WIDTH-heroes[0].size){
                        herorepaint(display,gc,window,heroes[0],0xFFFFFF);
                        heroes[0].x+=heroes[0].speed_x;
                        herorepaint(display,gc,window,heroes[0],heroes[0].color);
                    }
             }
            break;
         }
   
 flag=cheker(heroes);        
  }
}



