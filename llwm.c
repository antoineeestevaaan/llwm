#include <X11/Xlib.h>
#include <stdlib.h>
#include <stdbool.h>

#include "x/xkeycodes.h"
#include "x/events.h"
#include "macros.h"
#include "ll.h"

#define Super Mod4Mask

Display *d;
Window r;
XEvent e;

typedef struct {
    Window id;
    bool is_mapped;
    bool is_configured;
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

int string_to_keycode(const char *key) { return XKeysymToKeycode(d, XStringToKeysym(key)); }
void grab_key(int modifiers, const char *key) {
    logln("register %s", key);
    XGrabKey(d, string_to_keycode(key), modifiers, r, 1, 1, 1);
}

int main() {
    d = XOpenDisplay(0);
    r = DefaultRootWindow(d);

    ll_cell_t *windows = cycle_init();

    logln("display: %d", d);
    logln("root   : %d", r);

    XSelectInput(d, r, SubstructureRedirectMask | SubstructureNotifyMask);

    grab_key(Super, "q");
    grab_key(Super, "n");
    grab_key(Super, "t");
    grab_key(Super, "z");

    bool running = true;

    while (!XNextEvent(d, &e) && running) {
        xevent_logln(e);
        switch (e.type) {
            case MapRequest:
                cycle_insert(&windows, e.xmaprequest.window);
                xwindow_map(windows->value, 2, 0);

                break;
            case ConfigureRequest:
                if (windows->value != e.xconfigurerequest.window) {
                    log(
                        "ERROR: current window (%d) is not the same as requested (%d)",
                        windows->value,
                        e.xconfigurerequest.window
                    );
                    break;
                }

                xwindow_configure(windows->value, 0, 0, 1920, 1080);

                break;
            case DestroyNotify:
                if (windows->value != e.xdestroywindow.window) {
                    log(
                        "ERROR: current window (%d) is not the same as requested (%d)",
                        windows->value,
                        e.xdestroywindow.window
                    );
                    break;
                }

                cycle_remove(&windows);
                XCirculateSubwindowsUp(d, r);
                if (!cycle_is_empty(windows)) xwindow_focus(windows->value, 2, 0);

                break;
            case KeyPress:
                switch (e.xkey.keycode) {
                    case X11_n:
                        logln("n");

                        cycle_forward(&windows);
                        XCirculateSubwindowsUp(d, r);
                        if (!cycle_is_empty(windows)) xwindow_focus(windows->value, 2, 0);

                        break;
                    case X11_q:
                        logln("q");

                        if (!cycle_is_empty(windows)) xwindow_kill(windows->value);
                        cycle_remove(&windows);
                        XCirculateSubwindowsUp(d, r);
                        if (!cycle_is_empty(windows)) xwindow_focus(windows->value, 2, 0);

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

        cycle_print(windows);
    }

    XCloseDisplay(d);
}
