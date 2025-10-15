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
    bool is_init;
} window_t;

window_t window_empty();
bool window_is_ready(window_t win);
void window_map(window_t win, int revert_to, Time time);
void window_configure(window_t win, int x, int y, int w, int h);
void window_kill(window_t win);
void window_focus(window_t win, int revert_to, Time time);

window_t window_empty() { return (window_t){ .id=-1, .is_init=false }; }
bool window_is_ready(window_t win) { return win.id != -1 && win.is_init; }
void window_map(window_t win, int revert_to, Time time) {
    logln("    XMapWindow(%d)", win.id);
    XMapWindow(d, win.id);
    window_focus(win, revert_to, time);
}
void window_configure(window_t win, int x, int y, int w, int h) {
    logln("    XMoveResizeWindow(%d, %d, %d, %d, %d)", win.id, x, y, w, h);
    XMoveResizeWindow(d, win.id, x, y, w, h);
}
void window_kill(window_t win) {
    logln("    XKillClient(%d)", win.id);
    XKillClient(d, win.id);
}
void window_focus(window_t win, int revert_to, Time time) {
    logln("    XSetInputFocus(%d, %d, %d)", win.id, revert_to, time);
    XSetInputFocus(d, win.id, revert_to, time);
}

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

                my_window.id = e.xmaprequest.window;
                window_map(my_window, 2, 0);

                break;
            case ConfigureRequest:
                if (window_is_ready(my_window)) {
                    logln("    already a window (%d)", my_window.id);
                    break;
                }

                if (my_window.id != e.xconfigurerequest.window) {
                    logln(
                        "    requested window (%d) different than current window (%d)",
                        e.xconfigurerequest.window,
                        my_window.id
                    );
                    break;
                }

                window_configure(my_window, 0, 0, 1920, 1080);
                my_window.is_init = true;

                break;
            case DestroyNotify:
                if (e.xdestroywindow.window == my_window.id) {
                    logln("    destroy %d", my_window.id);
                    my_window = window_empty();
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
                        window_focus(my_window, 2, 0);
                        break;
                    case X11_q:
                        logln("q");
                        if (!window_is_ready(my_window)) {
                            logln("    no window");
                            break;
                        }

                        window_kill(my_window);
                        my_window = window_empty();

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
