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

enum status{
    
    begin,
    line,
    end
};

struct Cpoint{
    int x,y;
    int color;
    status flag;
};

typedef struct Cpoint Cpoint;
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

void create_color_window(Display *d,Window rootwindow, Window &w,int screen_number, int &x, int y, int color){
    int size=20,distance=7;
    int border_widght=2;
    w = XCreateSimpleWindow ( d,
                               rootwindow,
                               x,y, size, size, border_widght,
                               BlackPixel ( d, screen_number ),
                               WhitePixel ( d, screen_number ) );
   XSetWindowBackground(d, w, color); 
   XSelectInput ( d, w, ExposureMask | ButtonPressMask) ;
   x+=size+distance;
    
}

void set_pero_color(Display *display, GC gc, Window color_window[], int color[],int index, int &selected_window){
    XSetForeground ( display, gc,  color[index]);
    XSetWindowBorder(display, color_window[index], 0xFFD700);
    XSetWindowBorder(display, color_window[selected_window], 0x808080);
    selected_window=index;
}

int main(int argc, char *argv[])
{
 Display * display;
 int screen_number;
 GC gc;
 XEvent report;
 Window window,color_window[4];
 XEvent event;
Window root_return, child_return;
Cpoint *points;
int root_x_return, root_y_return,win_x_return,win_y_return;
int color[4]={0x000000,0xFF0000,0x008000,0x0000FF},border=5,selected_window=0;
int left_x=2;
int flag=1,x0,y0,size=0;
unsigned int mask_return;
points=(Cpoint *)malloc(sizeof(Cpoint));
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
    
    
 XSelectInput ( display, window, ExposureMask | KeyPressMask | ButtonPressMask  | ButtonReleaseMask | ButtonMotionMask  | StructureNotifyMask) ;
 XMapWindow ( display, window );
  for(int i=0;i<4;++i){
     create_color_window(display,window, color_window[i],screen_number,left_x,5,color[i]);
     XMapWindow ( display, color_window[i] );
}
XSetWindowBorder(display, color_window[0], 0xFFD700);
for (int i=1;i<4;++i)
    XSetWindowBorder(display, color_window[i], 0x808080);
 while ( 1 ) {
  XNextEvent ( display, &report );
  switch ( report.type ) {
    case Expose :
     if ( report.xexpose.count != 0 )
      break;
     gc = XCreateGC ( display, window, 0 , NULL );
     XSetForeground ( display, gc,  color[selected_window]);
     break;
     case ButtonPress:
         if(report.xbutton.window != window){
                if (report.xbutton.window==color_window[0])
                    set_pero_color(display, gc,color_window,color,0,selected_window);
                else if (report.xbutton.window==color_window[1])
                    set_pero_color(display, gc,color_window,color,1,selected_window);
                else if (report.xbutton.window==color_window[2])
                    set_pero_color(display, gc,color_window,color,2,selected_window);
                else
                    set_pero_color(display, gc,color_window,color,3,selected_window);
             
        }
        else{
         size++;
         points=(Cpoint*)realloc(points,size*sizeof(Cpoint));
         x0=report.xbutton.x;
         y0=report.xbutton.y;
         points[size-1].x=x0;
         points[size-1].y=y0;
         points[size-1].color=color[selected_window];
         points[size-1].flag=begin;
         flag=1;}
         break;
    case ButtonRelease:
        flag=0;
        points[size-1].flag=end;
        break;
    case MotionNotify:
        if(flag){
            XDrawLine(display, window, gc,x0,y0,report.xmotion.x,report.xmotion.y);
            size++;
            points=(Cpoint*)realloc(points,size*sizeof(Cpoint));
            x0=report.xmotion.x;
            y0=report.xmotion.y;
            points[size-1].x=x0;
            points[size-1].y=y0;
            points[size-1].color=color[selected_window];
            points[size-1].flag=line;
            
            
        }
        break;
    case ConfigureNotify:
        for (int i=0;i<size-1;++i){
            if(points[i].flag==begin)
                XSetForeground ( display, gc,  points[i].color);
            if(points[i].flag!=end){
                XDrawLine(display, window, gc,points[i].x,points[i].y,points[i+1].x,points[i+1].y);}
        }
        break;
         }
    
  }
  free(points);
 }

// XQueryPointer СЃС‡РёС‚Р°РµС‚ РєРѕРѕСЂРґРёРЅР°С‚С‹!
// XQueryPointer(display, window,root_return,child_return,root_x_return,root_y_return,win_x_return,win_y_return,mask_return); 
// РєРёСЃС‚СЊ x0=report.xmotion.x_root;
 //   y0=report.xmotion.y_root;
