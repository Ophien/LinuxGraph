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
#include "Ars.h"

//--------------------------GLOBAL INCLUDES-----------------------
#include <GL/glew.h>
#include <GLFW/glfw3.h>

//--------------------------MACROS--------------------------------

//--------------------------DIRECTIVES----------------------------

//--------------------------NAMESPACES----------------------------
using namespace Ars;
using namespace std;

GLfloat   width;
GLfloat   height;
GLfloat   zNear;
GLfloat   zFar;
bool      fixedSize;
string    windowName;

GLFWwindow* window;

char version[128] = "V1.1";

void key_callback(GLFWwindow* window, int key, int scancode, int act, int mods)
{
    if(key > 255)
        _charPressed[key] = act;

    _pressedC    = key;
}
//----------------------------------------------------------------------------------------------
void charFunc(GLFWwindow* window, unsigned int c)
{
    if(c < 255)
        _charPressed[c] = GLFW_PRESS;

    _pressedC    = c;
}
//----------------------------------------------------------------------------------------------
void mouseButtons(GLFWwindow* window, int button, int state, int action)
{
    // Buttons states
    _mouseButtonState[button]         = state;

    /** Main program callback **/
    mouseClick();
}
//----------------------------------------------------------------------------------------------
Vector2f ScreenToWorld(int x, int y)
{
    GLint viewport[4];
    GLdouble modelview[16];
    GLdouble projection[16];
    GLfloat winX, winY, winZ;
    GLdouble posX, posY, posZ;

    glGetDoublev( GL_MODELVIEW_MATRIX, modelview );
    glGetDoublev( GL_PROJECTION_MATRIX, projection );
    glGetIntegerv( GL_VIEWPORT, viewport );

    winX = (float)x;
    winY = (float)viewport[3] - (float)y;
    glReadPixels( x, int(winY), 1, 1, GL_DEPTH_COMPONENT, GL_FLOAT, &winZ );

    gluUnProject(winX, winY, winZ, modelview, projection, viewport, &posX, &posY, &posZ);

    return Vector2f(posX, posY);
}
//----------------------------------------------------------------------------------------------
void changeSize(GLFWwindow* window, int w, int h)
{
    //  windowSize.x = w;
    //  windowSize.y = h;
    // glMatrixMode  (GL_PROJECTION);
    // glLoadIdentity();
    glViewport    (0,0,w,h);
    //glOrtho       (0, w, 0, h,zNear,zFar);
    //glMatrixMode  (GL_MODELVIEW);

    //printf("MERDA\n");
}
//----------------------------------------------------------------------------------------------
void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
    glViewport(0, 0, width, height);
}
//----------------------------------------------------------------------------------------------
void mouseMove(GLFWwindow* window, double x, double y)
{
    // Set window position
    _mousePosition2D_window.x = (float)x;
    _mousePosition2D_window.y = (float)y;

    _mousePosition2D_world    = ScreenToWorld(x, y);

    // Redraw window content
    //glutPostRedisplay();
    mouseMoveExternal();

    // Set world position
    // printf("Window position: x: %f y: %f \n", mousePosition2D_window.x, mousePosition2D_window.y);
    // printf("World  position: x: %f y: %f \n", mousePosition2D_world.x , mousePosition2D_world.y );
}

//----------------------------------------------------------------------------------------------
int main(int argc, char *argv[])
{
    /** Get program path to load all assets **/
    _contentPath            = string(argv[0]);
    unsigned found          = _contentPath.find_last_of("/\\");
    _contentPath            = _contentPath.substr(0,found) + "/content/";

    if(argc > 2)
    {
        int w = atoi(argv[1]);
        int h = atoi(argv[2]);

        _windowWidth  = w;
        _windowHeight = h;

        if(w < 1024)
            _windowWidth  = 1024;

        if(h < 720)
            _windowHeight = 720;
    }else{
        _windowWidth      = 1024;
        _windowHeight     = 768;
    }

    zNear      = 0.00001f;
    zFar       = 10000.0f;
    windowName = "ARSGraph";

    /** Vsync test **/
    putenv( (char *) "__GL_SYNC_TO_VBLANK=1" );

    if (!glfwInit())
        exit(EXIT_FAILURE);

    /** Configure window hint here **/
    //glfwWindowHint(GLFW_SAMPLES, 0);
    glfwWindowHint(GLFW_RESIZABLE, GL_FALSE);
    //glfwSwapInterval(0);
    //glfwSwapInterval(2);

    /** Load GLFW WINDOW **/
    window = glfwCreateWindow(_windowWidth, _windowHeight, windowName.c_str(), NULL, NULL);

    /** Initializa GTK **/
    gtk_init (&argc, &argv);

    if (!window)
    {
        glfwTerminate();
        return -1;
    }

    /** Para auxilio **/
    _win = (void*)window;

    /* Make the window's context current */
    glfwMakeContextCurrent(window);

    //glfwWindowHint(GLFW_REFRESH_RATE, 60);

    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_HIDDEN);

    glfwSetWindowSizeCallback     (window, changeSize);
    glfwSetCursorPosCallback      (window, mouseMove );
    glfwSetMouseButtonCallback    (window, mouseButtons);
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
    glfwSetCharCallback           (window, charFunc);
    glfwSetKeyCallback            (window, key_callback);

    //glfwWindowHint( GLFW_RESIZABLE, GL_FALSE);
    //changeSize(window, width, height);

    //glClearDepth(1.0f);
    //glDepthFunc(GL_LESS_OR_EQUAL);
    //glEnable(GL_DEPTH_TEST);

    glMatrixMode  (GL_PROJECTION);
    glLoadIdentity();
    glViewport    (0, 0   , _windowWidth, _windowHeight);
    glOrtho       (0,_windowWidth, 0, _windowHeight, zNear, zFar);
    glMatrixMode  (GL_MODELVIEW);

    // Print welcome msg console
    printf("ARSGraph %s\n", version);

    /** LOAD ALL MAIN PROGRAM DATA **/
    load();

    /* Loop until the user closes the window */
    while (!glfwWindowShouldClose(window))
    {
        float start = glfwGetTime();

        /* Update */
        update();

        /* Render here */
        draw  ();

        /* Swap front and back buffers */
        glfwSwapBuffers(window);

        /** Important **/
        for(int i = 0; i < 3  ; i ++)_mousePreviousButtonState[i] = _mouseButtonState[i];

        for(int i = 0; i < ARS_TOTAL_CHARACTERS; i ++)
        {
            _charPressedPrevious[i] = _charPressed[i];
            _charPressed [i] = GLFW_RELEASE;
        }

        /** Update GTK system **/
        while (gtk_events_pending())
            gtk_main_iteration();

        /* Poll for and process events */
        glfwPollEvents();

        float endt = glfwGetTime();
        _deltaTime = endt - start;
    }

    glfwTerminate();

    return 0;
}
