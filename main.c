#include <X11/Xlib.h>
#include <X11/Xutil.h>
#include <stdio.h>
#include <stdbool.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <signal.h>
#include <errno.h>

#define MAX(a, b) (a > b ? a : b)

Display *display;
Window root;
XButtonEvent start;
XWindowAttributes attr;

Window frame;
Window child; 

void keypress();
void buttonpress();
void maprequest();
void configurerequest();
void buttonrelease();
void motionnotify();

static void (*handler[LASTEvent]) (XEvent *) = {
	[ButtonPress] = buttonpress,
	[ButtonRelease] = buttonrelease,
	[ConfigureRequest] = configurerequest,
	[KeyPress] = keypress,
	[KeyRelease] = keypress,
	[MapRequest] = maprequest,
	[MotionNotify] = motionnotify,
};

bool wm_detected;

void onwmdetected() { wm_detected = true; }

void onerror() {}

void buttonpress(XEvent ev)
{
	printf("buttonpress()\n");
	XRaiseWindow(display, ev.xkey.subwindow);
	XGetWindowAttributes(display, ev.xbutton.subwindow, &attr);
	start = ev.xbutton;

	Window root_return;
	Window parent_return;
	Window child_return;
	int nchild_return;

	XQueryTree(display, frame, &root_return, &parent_return, &child_return, &nchild_return);


}

void keypress(XEvent ev)
{
//	XRaiseWindow(display, ev.xkey.subwindow);

	// mod + d
	if (ev.xkey.keycode == 40) {
		setsid();

		if (fork() == 0) {
			printf("fork()\n");
			char *dmenucmd[] = { "dmenu_run", NULL };
			execv("/usr/bin/dmenu_run", dmenucmd);
			perror("execv failed\n");
			exit(0);
		}
	}
}

void motionnotify(XEvent ev)
{
	printf("motionnotify()\n");
	XRaiseWindow(display, ev.xkey.subwindow);
	int xdiff = ev.xbutton.x_root - start.x_root;
	int ydiff = ev.xbutton.y_root - start.y_root;
	XMoveResizeWindow(display, start.subwindow,
	attr.x + (start.button==1 ? xdiff : 0),
	attr.y + (start.button==1 ? ydiff : 0),
	MAX(1, attr.width + (start.button==3 ? xdiff : 0)),
	MAX(1, attr.height + (start.button==3 ? ydiff : 0)));
}

void buttonrelease(XEvent ev)
{
	start.subwindow = None;
	printf("x = %d\ny=%d\n", attr.x, attr.y);
}

void configurerequest(XEvent ev)
{
	XWindowChanges changes;
	// Copy fields from e to changes.
	changes.x = ev.xconfigurerequest.x;
	changes.y = ev.xconfigurerequest.y;
	changes.width = ev.xconfigurerequest.width;
	changes.height = ev.xconfigurerequest.height;
	changes.border_width = ev.xconfigurerequest.border_width;
	changes.sibling = ev.xconfigurerequest.above;
	changes.stack_mode = ev.xconfigurerequest.detail;
	// Grant request by calling XConfigureWindow().
	XConfigureWindow(display, ev.xconfigurerequest.window, ev.xconfigurerequest.value_mask, &changes);
}

void maprequest(XEvent ev)
{
	XWindowAttributes x_window_attrs;
	XGetWindowAttributes(display, ev.xconfigurerequest.window, &x_window_attrs);

	XMapWindow(display, ev.xconfigurerequest.window);
	XSync(display, false);
}

void run()
{
	for (;;) {
		XEvent ev;
		XNextEvent(display, &ev);

		if (ev.xany.window == child) {
			if (ev.type == Expose)
				printf("CHILDWIN EXPOSE\n");
			else if (ev.type == ButtonPress)
				printf("CHILDWIN BUTTONPRESS\n");
		}	

		if (handler[ev.type])
			handler[ev.type](&ev);
	}
}



int main()
{
	display = XOpenDisplay(0x0);

	if (display == NULL) {
		fprintf(stderr, "Unable to open display\n");
		return 0;
	}

	root = DefaultRootWindow(display);

	wm_detected = false;
	XSetErrorHandler(&onwmdetected);
	XSelectInput(display, root, SubstructureRedirectMask | SubstructureNotifyMask);
	XSync(display, false);

	if (wm_detected) {
		fprintf(stderr, "Another wm is running\n");
//		return 0;
	}

	XSetErrorHandler(&onerror);

	XGrabKey(display, XKeysymToKeycode(display, XStringToKeysym("F1")), Mod1Mask,
       	DefaultRootWindow(display), True, GrabModeAsync, GrabModeAsync);

	XGrabKey(display, XKeysymToKeycode(display, XStringToKeysym("d")), Mod1Mask,
       	DefaultRootWindow(display), True, GrabModeAsync, GrabModeAsync);

    XGrabButton(display, 1, Mod1Mask, DefaultRootWindow(display), True,
            ButtonPressMask|ButtonReleaseMask|PointerMotionMask, GrabModeAsync, GrabModeAsync, None, None);
    XGrabButton(display, 3, Mod1Mask, DefaultRootWindow(display), True,
            ButtonPressMask|ButtonReleaseMask|PointerMotionMask, GrabModeAsync, GrabModeAsync, None, None);

	start.subwindow = None;

	frame = XCreateSimpleWindow(display, root, 200, 200, 500, 300, 2, 0xFFFFFF, 0xABCDEF);

	XSelectInput(
        display,
        frame,
        ExposureMask | KeyPressMask);
	XMapWindow(display, frame);


	child = XCreateSimpleWindow(display, frame, 20, 20, 50, 30, 2, 0xFFFFFF, 0x32CD32);
	XSelectInput(display, child,
        ExposureMask | KeyPressMask | ButtonPressMask | ButtonReleaseMask);
	XMapWindow(display, child);

	XSync(display, false);

	run();

	XCloseDisplay(display);
}
