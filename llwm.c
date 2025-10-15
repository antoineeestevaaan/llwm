#include <X11/Xlib.h>
#include <stdlib.h>
#include <stdbool.h>

#include "x/xkeycodes.h"

#define Super Mod4Mask

Display *d;
Window r;
XEvent e;

#ifdef DEBUG
#include <stdio.h>
#define        log(fmt, ...) printf(fmt, ##__VA_ARGS__)
#define        logln(fmt, ...) printf(fmt "\n", ##__VA_ARGS__)
#else // DEBUG
#define        log(fmt, ...) ((void)0)
#define        logln(fmt, ...) ((void)0)
#endif // DEBUG

#define bool_to_cstr(b) b ? "true" : "false"

typedef struct {
    Window id;
    int x, y, w, h;
    bool is_init;
} window_t;

window_t window_empty() { return (window_t){ .id=-1, .is_init=false }; }
bool window_is_ready(window_t win) { return win.id != -1 && win.is_init; }

void xevent_print(XEvent e) {
    switch (e.type) {
        case ConfigureRequest:
            logln(
                "ConfigureRequest (%d: serial=%ld, send_event=%s, event=%d, win=%d, x=%d, y=%d, width=%d, height=%d, above=%d, override_redirect=%s):",
                e.xconfigure.type,
                e.xconfigure.serial,
                bool_to_cstr(e.xconfigure.send_event),
                e.xconfigure.event,
                e.xconfigure.window,
                e.xconfigure.x,
                e.xconfigure.y,
                e.xconfigure.width,
                e.xconfigure.height,
                e.xconfigure.border_width,
                e.xconfigure.above,
                bool_to_cstr(e.xconfigure.override_redirect)
            );
            break;
        case MapRequest:
            logln(
                "MapRequest (%d: serial=%ld, send_event=%s, parent=%d, win=%d): ",
                e.xmaprequest.type,
                e.xmaprequest.serial,
                bool_to_cstr(e.xmaprequest.send_event),
                e.xmaprequest.parent,
                e.xmaprequest.window
            );
            break;
        case KeyPress:
            log("KeyPress (%d: keycode=%d, win=%d, subwin=%d): ", e.xkey.type, e.xkey.keycode, e.xkey.window, e.xkey.subwindow);
            break;
        case KeyRelease   : logln("KeyRelease (%d)", e.type); break;
        case ClientMessage:
            log(
                "ClientMessage (%d: serial=%ld, send_event=%s, win=%d, format=%d, ",
                e.xclient.type,
                e.xclient.serial,
                bool_to_cstr(e.xclient.send_event),
                // e.xclient.message_type,
                e.xclient.window,
                e.xclient.format
            );
            log("data: [");
            switch (e.xclient.format) {
                case 8:
                    for (int i = 0; i < 20; i++) log("%c%s", e.xclient.data.b[i], i == 19 ? "" : ", ");
                    break;
                case 16:
                    for (int i = 0; i < 10; i++) log("%d%s", e.xclient.data.s[i], i == 9 ? "" : ", ");
                    break;
                case 32:
                    for (int i = 0; i < 5; i++) log("%ld%s", e.xclient.data.l[i], i == 4 ? "" : ", ");
                    break;
                default:
                    logln("ERROR: unknown xclient format %d", e.xclient.format);
                    break;
            }
            logln("])");
            break;
        case MappingNotify:
            logln("MappingNotify (%d: serial=%ld, send_event=%s, win=%d, request=%s, first_keycode=%d, count=%d)",
                e.xmapping.type,
                e.xmapping.serial,
                bool_to_cstr(e.xmapping.send_event),
                e.xmapping.window,
                e.xmapping.request == MappingModifier ? "MappingModifier" : e.xmapping.request == MappingPointer ? "MappingPointer" : e.xmapping.request == MappingKeyboard ? "MappingKeyboard" : "???",
                e.xmapping.count,
                e.xmapping.first_keycode
            );
            break;
        case ButtonPress:      logln("ButtonPress (???)"); break;
        case ButtonRelease:    logln("ButtonRelease (???)"); break;
        case MotionNotify:     logln("MotionNotify (???)"); break;
        case EnterNotify:      logln("EnterNotify (???)"); break;
        case LeaveNotify:      logln("LeaveNotify (???)"); break;
        case FocusIn:          logln("FocusIn (???)"); break;
        case FocusOut:         logln("FocusOut (???)"); break;
        case KeymapNotify:     logln("KeymapNotify (???)"); break;
        case Expose:           logln("Expose (???)"); break;
        case GraphicsExpose:   logln("GraphicsExpose (???)"); break;
        case NoExpose:         logln("NoExpose (???)"); break;
        case VisibilityNotify: logln("VisibilityNotify (???)"); break;
        case CreateNotify:     logln("CreateNotify (???)"); break;
        case DestroyNotify:
            logln(
                "DestroyNotify (%d: serial=%ld, send_event=%s, event=%d, window=%d)",
                e.xdestroywindow.type,
                e.xdestroywindow.serial,
                bool_to_cstr(e.xdestroywindow.send_event),
                e.xdestroywindow.event,
                e.xdestroywindow.window
            );
            break;
        case UnmapNotify:      logln("UnmapNotify (???)"); break;
        case MapNotify:        logln("MapNotify (???)"); break;
        case ReparentNotify:   logln("ReparentNotify (???)"); break;
        case ConfigureNotify:  logln("ConfigureNotify (???)"); break;
        case GravityNotify:    logln("GravityNotify (???)"); break;
        case ResizeRequest:    logln("ResizeRequest (???)"); break;
        case CirculateNotify:  logln("CirculateNotify (???)"); break;
        case CirculateRequest: logln("CirculateRequest (???)"); break;
        case PropertyNotify:   logln("PropertyNotify (???)"); break;
        case SelectionClear:   logln("SelectionClear (???)"); break;
        case SelectionRequest: logln("SelectionRequest (???)"); break;
        case SelectionNotify:  logln("SelectionNotify (???)"); break;
        case ColormapNotify:   logln("ColormapNotify (???)"); break;
        case GenericEvent:     logln("GenericEvent (???)"); break;
        case LASTEvent:        logln("LASTEvent (???)"); break;
        default:               logln("???"); break;
    }
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
