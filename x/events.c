#include <X11/Xlib.h>
#include "../macros.h"

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

