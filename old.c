void run()
{
	for (;;) {
		XEvent ev;


		XNextEvent(display, &ev);

		if(ev.type == KeyPress && ev.xkey.subwindow != None)
            XRaiseWindow(display, ev.xkey.subwindow);
        else if(ev.type == ButtonPress && ev.xbutton.subwindow != None)
        {
		XRaiseWindow(display, ev.xkey.subwindow);
            XGetWindowAttributes(display, ev.xbutton.subwindow, &attr);
            start = ev.xbutton;
        }
        else if(ev.type == MotionNotify && start.subwindow != None)
        {
            XRaiseWindow(display, ev.xkey.subwindow);
            int xdiff = ev.xbutton.x_root - start.x_root;
            int ydiff = ev.xbutton.y_root - start.y_root;
            XMoveResizeWindow(display, start.subwindow,
                attr.x + (start.button==1 ? xdiff : 0),
                attr.y + (start.button==1 ? ydiff : 0),
                MAX(1, attr.width + (start.button==3 ? xdiff : 0)),
                MAX(1, attr.height + (start.button==3 ? ydiff : 0)));
        }
        else if(ev.type == ButtonRelease) {
            start.subwindow = None;


	    printf("x = %d\ny=%d\n", attr.x, attr.y);
	}


	else if (ev.type == ConfigureRequest) {
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

	else if (ev.type == MapRequest) {
		XWindowAttributes x_window_attrs;
		XGetWindowAttributes(display, ev.xconfigurerequest.window, &x_window_attrs);


	//	Window frame = XCreateSimpleWindow(display, root, x_window_attrs.x, x_window_attrs.x,x_window_attrs.width,x_window_attrs.height, 2, 0xFFFFFF, 0xABCDEF);
//		Window frame = XCreateSimpleWindow(display, root, 200, 200, x_window_attrs.width,x_window_attrs.height, 2, 0xFFFFFF, 0xABCDEF);


//		 XSelectInput(
 //     display,
  //    frame,
   //   SubstructureRedirectMask | SubstructureNotifyMask);
////		 XAddToSaveSet(display, ev.xconfigurerequest.window);

//		 XMapWindow(display, frame);

		 XMapWindow(display, ev.xconfigurerequest.window);

		 XSync(display, false);


	}
	}


}
