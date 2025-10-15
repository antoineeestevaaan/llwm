#include <X11/Xlib.h>
#include <stdlib.h>
#include <stdbool.h>

#include "x/xkeycodes.h"
#include "x/events.h"
#include "macros.h"

#define Super Mod4Mask

Display *d;
Window r;
XEvent e;

typedef struct {
    Window id;
    int x, y, w, h;
    bool is_init;
} window_t;

window_t window_empty() { return (window_t){ .id=-1, .is_init=false }; }
bool window_is_ready(window_t win) { return win.id != -1 && win.is_init; }


int string_to_keycode(const char *key) { return XKeysymToKeycode(d, XStringToKeysym(key)); }
void grab_key(int modifiers, const char *key) {
    logln("register %s", key);
    XGrabKey(d, string_to_keycode(key), modifiers, r, 1, 1, 1);
}

int main() {
    d = XOpenDisplay(0);
    r = DefaultRootWindow(d);

    window_t my_window = window_empty();

    logln("display: %d", d);
    logln("root   : %d", r);

    XSelectInput(d, r, SubstructureRedirectMask | SubstructureNotifyMask);

    grab_key(Super, "q");
    grab_key(Super, "n");
    grab_key(Super, "t");
    grab_key(Super, "z");

    Window win;

    bool running = true;

    while (!XNextEvent(d, &e) && running) {
        xevent_print(e);
        switch (e.type) {
            case MapRequest:
                if (window_is_ready(my_window)) {
                    logln("    already a window (%d)", my_window.id);
                    break;
                }

                win = e.xmaprequest.window;

                logln("    XMapWindow(%d)", win);
                XMapWindow(d, win);
                logln("    XSetInputFocus(%d, %d, %d)", win, 2, 0);
                XSetInputFocus(d, win, 2, 0);

                my_window.id = win;

                break;
            case ConfigureRequest:
                if (window_is_ready(my_window)) {
                    logln("    already a window (%d)", my_window.id);
                    break;
                }

                win = e.xmaprequest.window;
                int x = 0, y = 0;
                int w = 1920, h = 1080;

                logln("    XMoveResizeWindow(%d, %d, %d, %d, %d)", win, x, y, w, h);
                XMoveResizeWindow(d, win, x, y, w, h);

                my_window.x = x;
                my_window.y = y;
                my_window.w = w;
                my_window.h = h;
                my_window.is_init = true;

                break;
            case DestroyNotify:
                if (e.xdestroywindow.window == my_window.id) {
                    logln("    destroy %d", my_window.id);
                    my_window.id = -1;
                    my_window.is_init = false;
                }
                break;
            case KeyPress:
                switch (e.xkey.keycode) {
                    case X11_n:
                        logln("n");
                        if (!window_is_ready(my_window)) {
                            logln("    no window");
                            break;
                        }

                        XCirculateSubwindowsUp(d, r);
                        logln("    XSetInputFocus(%d, %d, %d)", my_window.id, 2, 0);
                        XSetInputFocus(d, my_window.id, 2, 0);
                        break;
                    case X11_q:
                        logln("q");
                        if (!window_is_ready(my_window)) {
                            logln("    no window");
                            break;
                        }

                        logln("    XKillClient(%d)", my_window.id);
                        XKillClient(d, my_window.id);

                        my_window.id = -1;
                        my_window.is_init = false;

                        break;
                    case X11_t:
                        logln("t");
                        system("alacritty &");
                        break;
                    case X11_z:
                        logln("z");
                        running = false;
                        break;
                    default:
                        logln("???");
                        break;
                }
                break;
        }
    }

    XCloseDisplay(d);
}
