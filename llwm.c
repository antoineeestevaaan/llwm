#include <X11/Xlib.h>
#include <stdlib.h>
#include <stdbool.h>

#include "x/xkeycodes.h"
#include "x/events.h"
#include "macros.h"
#define NOB_IMPLEMENTATION
#define NOB_STRIP_PREFIX
#include "thirdparty/nob.h"

#define Super Mod4Mask

Display *d;
Window r;
XEvent e;

typedef struct {
    Window id;
    bool is_init;
} window_t;

void xwindow_map(Window win, int revert_to, Time time);
void xwindow_configure(Window win, int x, int y, int w, int h);
void xwindow_kill(Window win);
void xwindow_focus(Window win, int revert_to, Time time);

void xwindow_map(Window win, int revert_to, Time time) {
    logln("    XMapWindow(%d)", win);
    XMapWindow(d, win);
    xwindow_focus(win, revert_to, time);
}
void xwindow_configure(Window win, int x, int y, int w, int h) {
    logln("    XMoveResizeWindow(%d, %d, %d, %d, %d)", win, x, y, w, h);
    XMoveResizeWindow(d, win, x, y, w, h);
}
void xwindow_kill(Window win) {
    logln("    XKillClient(%d)", win);
    XKillClient(d, win);
}
void xwindow_focus(Window win, int revert_to, Time time) {
    logln("    XSetInputFocus(%d, %d, %d)", win, revert_to, time);
    XSetInputFocus(d, win, revert_to, time);
}

typedef struct {
    window_t *items;
    size_t count;
    size_t capacity;
} da_window_t;

void windows_println(da_window_t windows) {
    log("[");
    da_foreach(window_t, win, &windows) {
        log("%d%s ", win->id, win->is_init ? "*" : "");
    }
    logln("]");
}

int string_to_keycode(const char *key) { return XKeysymToKeycode(d, XStringToKeysym(key)); }
void grab_key(int modifiers, const char *key) {
    logln("register %s", key);
    XGrabKey(d, string_to_keycode(key), modifiers, r, 1, 1, 1);
}

int main() {
    d = XOpenDisplay(0);
    r = DefaultRootWindow(d);

    da_window_t windows = {0};
    Window id;

    logln("display: %d", d);
    logln("root   : %d", r);

    XSelectInput(d, r, SubstructureRedirectMask | SubstructureNotifyMask);

    grab_key(Super, "q");
    grab_key(Super, "n");
    grab_key(Super, "t");
    grab_key(Super, "z");

    bool running = true;

    while (!XNextEvent(d, &e) && running) {
        xevent_print(e);
        switch (e.type) {
            case MapRequest:
                id = e.xmaprequest.window;

                bool already_in = false;
                da_foreach(window_t, win, &windows) {
                    if (win->id == id) already_in = true;
                }
                if (already_in) {
                    logln("WARNING: window (%d) already in", id);
                    break;
                }

                window_t win = { .id = id, .is_init = false };
                xwindow_map(win.id, 2, 0);
                da_append(&windows, win);

                windows_println(windows);

                break;
            case ConfigureRequest:
                id = e.xconfigurerequest.window;

                bool found = false;
                window_t *requested;
                da_foreach(window_t, win, &windows) {
                    if (win->id == id) {
                        found = true;
                        requested = win;
                        break;
                    }
                }
                if (!found) {
                    log("ERROR: window (%d) not found in ", id);
                    windows_println(windows);
                    break;
                } else if (requested->is_init) {
                    log("WARNING: window (%d) already configured", id);
                    break;
                }

                xwindow_configure(requested->id, 0, 0, 1920, 1080);
                requested->is_init = true;

                windows_println(windows);

                break;
            case DestroyNotify:
                // if (e.xdestroywindow.window == my_window.id) {
                //     logln("    destroy %d", my_window.id);
                //     my_window = window_empty();
                // }
                break;
            case KeyPress:
                switch (e.xkey.keycode) {
                    case X11_n:
                        logln("n");
                        // if (!window_is_ready(my_window)) {
                        //     logln("    no window");
                        //     break;
                        // }
                        //
                        // XCirculateSubwindowsUp(d, r);
                        // xwindow_focus(my_window, 2, 0);
                        break;
                    case X11_q:
                        logln("q");
                        // if (!window_is_ready(my_window)) {
                        //     logln("    no window");
                        //     break;
                        // }
                        //
                        // xwindow_kill(my_window);
                        // my_window = window_empty();

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
