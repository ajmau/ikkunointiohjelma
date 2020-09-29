#include <X11/X.h>
#include <X11/Xlib.h>
#include <X11/Xutil.h>
#include <stdio.h>
#include <stdbool.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <signal.h>
#include <errno.h>

#define MAX(a, b) (a > b ? a : b) 
Display *display;
Window root;
XButtonEvent start;
XWindowAttributes attr;

struct win {
	Window child;
	Window frame;
};

struct win map[10];
unsigned int wins=0;

unsigned int childs[10];
unsigned int parents[10];
unsigned int buttons[10];

Window frame;
Window child; 
Window child2; 
Window closebutton;
Window minimizebutton;

void keypress();
void buttonpress();
void maprequest();
void configurerequest();
void buttonrelease();
void motionnotify();
void unmapnotify();
void destroynotify();

static void (*handler[LASTEvent]) (XEvent *) = {
	[ButtonPress] = buttonpress,
	[ButtonRelease] = buttonrelease,
	[ConfigureRequest] = configurerequest,
	[KeyPress] = keypress,
	[KeyRelease] = keypress,
	[MapRequest] = maprequest,
	[UnmapNotify] = unmapnotify,
	[DestroyNotify] = destroynotify,
	[MotionNotify] = motionnotify,
};

bool wm_detected;

void onwmdetected() { wm_detected = true; }

void onerror() {}

void print_lists()
{
	int i=0;
	for (i=0; i < 10; i++) 
		printf("parents[%d] = %d\n", i, parents[i]);
	for (i=0; i < 10; i++) 
		printf("childs[%d] = %d\n", i, childs[i]);
	for (i=0; i < 10; i++) 
		printf("buttons[%d] = %d\n", i, buttons[i]);
}

void destroynotify(XEvent ev)
{
	printf("destroynotify()\n");

	Window parent_ret;
	Window *child_ret;
	Window root_ret;
	int n;

	//print_lists();
	printf("window = %d\n", ev.xkey.window);
	int i;
	for (i=0; i < 10; i++) {
		if (ev.xkey.window == childs[i]) {
			XDestroyWindow(display, parents[i]);
			XDestroyWindow(display, childs[i]);
		}
	}
	XDestroyWindow(display, ev.xkey.window);
	XDestroyWindow(display, ev.xbutton.window);
	XDestroyWindow(display, ev.xbutton.subwindow);
}

void destroywindow(Window w)
{

	Window root_return;
        Window parent_return;
        Window* children_return;
        unsigned int nchildren_return;

	Window local_parent;
	int a;
	for (a=0; a < 10; a++) {
		//printf("frame = %d\nparents[%d] = %d\n", frame, a, parents[a]);
		if (frame == parents[a]) {
			local_parent = parents[a];
			XDestroyWindow(display, local_parent);
			break;
		}
	}
	
}

void unmapnotify(XUnmapEvent ev)
{
	printf("unmäppäääää\n");

	int i;

	for (i=0; i<10; i++) {
		if (ev.window == childs[i]) {
			printf("FOUND RIGHT WINDOW!!!!!!!!!!!!!\n");
			XUnmapWindow(display, parents[i]);
			XDestroyWindow(display, parents[i]);
		}
	}
}

void buttonpress(XEvent ev)
{
	//printf("buttonpress() ev.xkey.subwindow = %d\n", ev.xkey.subwindow);
	printf("buttonpress()\n");

	int i;
	for (i = 0; i < 10; i++) {
		if (ev.xany.window == buttons[i]) {
			printf("close\n");
			printf("%d\n", ev.xany.window);
			if (ev.type == ButtonPress) {
				//destroywindow(parents[i]);
				XDestroyWindow(display, parents[i]);

			}
		}
	}

	frame = ev.xkey.subwindow;
	//printf("frame on %d\n", frame);
	XRaiseWindow(display, ev.xkey.subwindow);
	XGetWindowAttributes(display, ev.xbutton.subwindow, &attr);
	start = ev.xbutton;
}

void keypress(XEvent ev)
{
	// mod + d

	if (ev.xkey.keycode == XStringToKeysym("F4")) {
	int i;
	for (i = 0; i < 10; i++) {
		if (ev.xany.window == buttons[i]) {
			printf("close\n");
			printf("%d\n", ev.xany.window);
			if (ev.type == ButtonPress) {
				//destroywindow(parents[i]);
				XDestroyWindow(display, parents[i]);

			}
		}
	}

	}

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

	XRaiseWindow(display, ev.xkey.subwindow);
	int xdiff = ev.xbutton.x_root - start.x_root;
	int ydiff = ev.xbutton.y_root - start.y_root;

	Window local_child;
	Window local_parent;

	int a;
	for (a=0; a < 10; a++) {
		if (frame == parents[a]) {
			local_child = childs[a];
			local_parent = parents[a];
			XRaiseWindow(display, local_child);
			break;
		}
	}

	XMoveResizeWindow(display, local_child,
			0,
			15,
			MAX(1, attr.width + (start.button==3 ? xdiff : 0)),
			MAX(1, attr.height + (start.button==3 ? ydiff : 0)));

	XMoveResizeWindow(display, local_parent,
			attr.x + (start.button==1 ? xdiff : 0),
			attr.y + (start.button==1 ? ydiff : 0),
			MAX(1, attr.width + (start.button==3 ? xdiff : 0)),
			MAX(1, attr.height + (start.button==3 ? ydiff : 0)));
}

void buttonrelease(XEvent ev)
{
	start.subwindow = None;
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

int counter=0;
void maprequest(XEvent ev)
{ 
	XWindowAttributes attrs;
	XGetWindowAttributes(display, ev.xconfigurerequest.window, &attrs);

	frame = XCreateSimpleWindow(display, root, attrs.x, attrs.y, attrs.width, attrs.height, 2, 0xFFFFFF, 0xABCDEF);

	XSelectInput(
			display,
			frame,
			ExposureMask | KeyPressMask);

	XMapWindow(display, frame);


	closebutton = XCreateSimpleWindow(display, frame, 0, 0, 30, 10, 2, 0xFFFFFF, 0xff0000);
	XSelectInput(display, closebutton,
			ExposureMask | KeyPressMask | ButtonPressMask | ButtonReleaseMask);

	minimizebutton = XCreateSimpleWindow(display, frame, 30, 0, 30, 10, 2, 0xFFFFFF, 0x32CD32);
	XSelectInput(display, child2,
	ExposureMask | KeyPressMask | ButtonPressMask | ButtonReleaseMask);


	XMapWindow(display, closebutton);
	XMapWindow(display, minimizebutton);

	XWindowAttributes x_window_attrs;
	XGetWindowAttributes(display, ev.xconfigurerequest.window, &x_window_attrs);

	XReparentWindow(display, ev.xconfigure.window, frame, 0, 15);

	XMapWindow(display, ev.xconfigurerequest.window);
	XSync(display, false);

	XGrabKey(display, XKeysymToKeycode(display, XStringToKeysym("F4")), Mod1Mask,
			DefaultRootWindow(display), True, GrabModeAsync, GrabModeAsync);

	child=ev.xconfigurerequest.window;
	parents[counter]=frame;
	buttons[counter]=closebutton;
	childs[counter++]=child;
	printf("child = %d\n", child);
}


void run()
{
	for (;;) {
		XEvent ev;
		XNextEvent(display, &ev);

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
		//return 0;
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

	run();

	XCloseDisplay(display);
}
