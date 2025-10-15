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

typedef struct {
    window_t *items;
    size_t count;
    size_t capacity;
} da_window_t;

void windows_logln(da_window_t windows) {
    log("[");
    da_foreach(window_t, win, &windows) {
        log(
            "%d-%s%s ",
            win->id,
            win->is_mapped     ? "M" : "_",
            win->is_configured ? "C" : "_"
        );
    }
    logln("]");
}

window_t *find_window(da_window_t windows, Window id) {
    da_foreach(window_t, win, &windows) {
        if (win->id == id) return win;
    }

    return NULL;
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
    Window current_win;
    Window id;
    window_t *p_win;

    // used to search for indices in arrays
    size_t current_idx, new_idx;

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
                p_win = find_window(windows, e.xmaprequest.window);
                if (p_win != NULL && p_win->is_mapped) {
                    logln("WARNING: window (%d) already in and mapped", e.xmaprequest.window);
                    break;
                }

                window_t win = {
                    .id            = e.xmaprequest.window,
                    .is_mapped     = true,
                    .is_configured = false,
                };
                xwindow_map(win.id, 2, 0);
                da_append(&windows, win);
                current_win = win.id;

                break;
            case ConfigureRequest:
                p_win = find_window(windows, e.xconfigurerequest.window);
                if (p_win == NULL) {
                    log("ERROR: window (%d) not found in ", e.xconfigurerequest.window);
                    windows_logln(windows);
                    break;
                } else if (p_win->is_configured) {
                    log("WARNING: window (%d) already configured", e.xconfigurerequest.window);
                    break;
                }

                xwindow_configure(p_win->id, 0, 0, 1920, 1080);
                p_win->is_configured = true;

                break;
            case DestroyNotify:
                p_win = find_window(windows, e.xdestroywindow.window);
                if (p_win == NULL) {
                    log("ERROR: window (%d) not found in ", e.xdestroywindow.window);
                    windows_logln(windows);
                    break;
                } else if (!p_win->is_mapped) {
                    log("WARNING: window (%d) is not mapped", e.xdestroywindow.window);
                    break;
                }

                logln("    destroy %d", p_win->id);
                p_win->is_mapped = false;
                p_win->is_configured = false;

                break;
            case KeyPress:
                switch (e.xkey.keycode) {
                    case X11_n:
                        logln("n");

                        p_win = find_window(windows, current_win);
                        if (p_win == NULL) {
                            log("ERROR: current window (%d) not found in ", current_win);
                            windows_logln(windows);
                            break;
                        } else if (!p_win->is_mapped) {
                            logln("ERROR: current window (%d) is not mapped", current_win);
                            break;
                        }

                        current_idx = (p_win - windows.items + 1) % windows.count;
                        new_idx = -1;

                        log("looking for window after %d in ", p_win->id);
                        windows_logln(windows);
                        while (true) {
                            if (windows.items[current_idx].is_mapped) {
                                new_idx = current_idx;
                                break;
                            }
                            if (windows.items[current_idx].id == current_win) {
                                break;
                            }
                            current_idx = (current_idx + 1) % windows.count;
                        }
                        if (new_idx == -1 || windows.items[new_idx].id == current_win) {
                            logln("ERROR: could not find any other active window after %d", current_win);
                            break;
                        }
                        logln("INFO: found %d", windows.items[current_idx].id);

                        current_win = windows.items[current_idx].id;

                        XCirculateSubwindowsUp(d, r);
                        xwindow_focus(current_win, 2, 0);
                        break;
                    case X11_q:
                        logln("q");

                        p_win = find_window(windows, current_win);
                        if (p_win == NULL) {
                            log("ERROR: current window (%d) not found in ", current_win);
                            windows_logln(windows);
                            break;
                        } else if (!p_win->is_mapped) {
                            logln("ERROR: current window (%d) is not mapped", current_win);
                            break;
                        }

                        xwindow_kill(current_win);

                        current_idx = (p_win - windows.items + 1) % windows.count;
                        new_idx = -1;

                        log("looking for window after %d in ", p_win->id);
                        windows_logln(windows);
                        while (true) {
                            if (windows.items[current_idx].is_mapped) {
                                new_idx = current_idx;
                                break;
                            }
                            if (windows.items[current_idx].id == current_win) {
                                break;
                            }
                            current_idx = (current_idx + 1) % windows.count;
                        }
                        if (new_idx == -1 || windows.items[new_idx].id == current_win) {
                            logln("ERROR: could not find any other active window after %d", current_win);
                            break;
                        }
                        logln("INFO: found %d", windows.items[current_idx].id);

                        current_win = windows.items[current_idx].id;

                        XCirculateSubwindowsUp(d, r);
                        xwindow_focus(current_win, 2, 0);

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

        log("%d ", current_win);
        windows_logln(windows);
    }

    XCloseDisplay(d);
}
