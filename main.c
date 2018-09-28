/*MIT License
*
*Copyright (c) 2018 Alysson Ribeiro da Silva
*
*Permission is hereby granted, free of charge, to any person obtaining a copy 
*of this software and associated documentation files (the "Software"), to deal 
*in the Software without restriction, including *without limitation the rights 
*to use, copy, modify, merge, publish, distribute, sublicense, and/or sell 
*copies of the Software, and to permit persons to whom the Software is furnished 
*to do so, subject *to the following conditions:
*
*The above copyright notice and this permission notice shall be included in all copies or substantial portions of the Software.
*
*THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, 
*EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, 
*FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. *IN NO EVENT SHALL THE AUTHORS 
*OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN 
*AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH 
*THE *SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
*/

//--------------------------LOCAL INCLUDES------------------------
#include "ArsMain.h"

//--------------------------GLOBAL INCLUDES-----------------------
#include <stdio.h>
#include <stdlib.h>

#include <GL/glx.h>    /* this includes the necessary X headers */
#include <GL/gl.h>

#include <X11/X.h>    /* X11 constant (e.g. TrueColor) */
#include <X11/keysym.h>

//--------------------------MACROS--------------------------------

//--------------------------DIRECTIVES----------------------------

//--------------------------NAMESPACES----------------------------
using namespace ArsMain;

static int snglBuf[] = {GLX_RGBA, GLX_DEPTH_SIZE, 16, None};
static int dblBuf[]  = {GLX_RGBA, GLX_DEPTH_SIZE, 16, GLX_DOUBLEBUFFER, None};

Display   *dpy;
Window     win;
GLboolean  doubleBuffer = GL_TRUE;

GLint width;
GLint height;

/** How window attributes **/
XVisualInfo         *vi;
Colormap             cmap;
XSetWindowAttributes swa;

GLXContext           cx;     /** To handle communication between Xwindow and GPU **/
XEvent               event;  /** To handle Xwindow events **/

/** Controll variable **/
int                  dummy;

void fatalError(char *message)
{
    fprintf(stderr, "main: %s\n", message);
    exit(1);
}

void configureWindow()
{
    width  = 1280;
    height = 720;
}

void configureXServer(int argc, char* argv[])
{
    /*** (1) open a connection to the X server ***/

    dpy = XOpenDisplay(NULL);

    if (dpy == NULL)
        fatalError("could not open display");

    /*** (2) make sure OpenGL's GLX extension supported ***/

    if(!glXQueryExtension(dpy, &dummy, &dummy))
        fatalError("X server has no OpenGL GLX extension");

    /*** (3) find an appropriate visual ***/

    /* find an OpenGL-capable RGB visual with depth buffer */
    vi = glXChooseVisual(dpy, DefaultScreen(dpy), dblBuf);

    if (vi == NULL)
    {
        vi = glXChooseVisual(dpy, DefaultScreen(dpy), snglBuf);
        if (vi == NULL) fatalError("no RGB visual with depth buffer");
        doubleBuffer = GL_FALSE;
    }

    if(vi->class != TrueColor)
        fatalError("TrueColor visual required for this program");

    /*** (4) create an OpenGL rendering context  ***/

    /* create an OpenGL rendering context */
    cx = glXCreateContext(dpy, vi, /* no shared dlists */ None,
                          /* direct rendering if possible */ GL_TRUE);
    if (cx == NULL)
        fatalError("could not create rendering context");

    /*** (5) create an X window with the selected visual ***/

    /* create an X colormap since probably not using default visual */
    cmap = XCreateColormap(dpy, RootWindow(dpy, vi->screen), vi->visual, AllocNone);

    swa.colormap     = cmap;
    swa.border_pixel = 0;
    swa.event_mask   = KeyPressMask    | ExposureMask
                       | ButtonPressMask | StructureNotifyMask;

    win = XCreateWindow(dpy, RootWindow(dpy, vi->screen), 0, 0, width, height, 0, vi->depth, InputOutput, vi->visual,
                        CWBorderPixel | CWColormap | CWEventMask, &swa);

    XSetStandardProperties(dpy, win, "main", "main", None,
                           argv, argc, NULL);

    /*** (6) bind the rendering context to the window ***/

    glXMakeCurrent(dpy, win, cx);

    /*** (7) request the X window to be displayed on the screen ***/

    XMapWindow(dpy, win);
}

void configureOpenGL()
{
    glEnable(GL_DEPTH_TEST);  /* enable depth buffering */
    glDepthFunc(GL_LESS);     /* pedantic, GL_LESS is the default */
    glClearDepth(1.0f);       /* pedantic, 1.0 is the default */

    /* frame buffer clears should be to black */
    glClearColor(0.0f, 1.0f, 0.0f, 0.0f);

    /* set up projection transform */
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    glFrustum(-1.0f, 1.0f, -1.0f, 1.0f, 1.0f, 10.0f);
    glViewport(0, 0, width, height);
}

void onScreenChange(int w, int h){
    width  = w;
    height = h;
    glViewport(0,0,width,height);
}

void onKeyPress(){
}

int main(int argc, char **argv)
{
    /** Configure program global state **/
    configureWindow();

    /** Configure xWindow and glcontext **/
    configureXServer(argc, argv);

    /** Configure initial openGL state **/
    configureOpenGL();

    /** Load app data from ArsMain namespace **/
    load();

    /** Program main loop **/

    while (True)
    {
        Bool hasEvent = XCheckWindowEvent(dpy, win, swa.event_mask, &event);

        do
        {
            switch (event.type)
            {
            case KeyPress:
            {
                KeySym     keysym;
                XKeyEvent *kevent;
                char       buffer[1];
                /* It is necessary to convert the keycode to a
                 * keysym before checking if it is an escape */
                kevent = (XKeyEvent *) &event;
                if (   (XLookupString((XKeyEvent *)&event,buffer,1,&keysym,NULL) == 1)
                        && (keysym == (KeySym)XK_Escape) )
                    exit(0);
                break;
            }
            case ConfigureNotify:
                onScreenChange(event.xconfigure.width,event.xconfigure.height);
                break;
            }
            hasEvent = XCheckWindowEvent(dpy, win, swa.event_mask, &event);
        }
        while(hasEvent);

        /** Main program update method **/
        update();

        /** Basic camera position configuration **/
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        glMatrixMode  (GL_PROJECTION);
        glLoadIdentity();
        glOrtho       (0, width, 0, height, 0.001f, 10000.0f);
        glMatrixMode  (GL_MODELVIEW);
        glLoadIdentity();

        /* move the camera back three units */
        glTranslatef  (0.0, 0.0, -3.0);

        /** Main program draw method **/
        draw();

        glXSwapBuffers(dpy, win);
    }

    return 0;
}
