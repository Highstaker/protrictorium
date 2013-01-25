#include <GL/glfw.h>

#include <iostream>
#include <string.h>
#include "ck_maths.h"
#include <string>
#include <stdlib.h>

//////////////////////
////GLOBALS
///////////////////

bool running;
bool refresh = false;//make this true every time the refresh of screen is required
int windowWidth = 500 ;
int windowHeight = 500;
float *pixel_data;
int line_thickness = 1;
//int *brush_buffer;
int buf_x =-1; int buf_y =-1;

// window close callback function
int closedWindow()
{
    running = false;
    return GL_TRUE;
}

int xyTO1d(int x, int y)
{
    return x + y * windowWidth;
}

void GLFWCALL resizeWindow(int width, int height);
void GLFWCALL mouseClick(int button ,int action);
void GLFWCALL mouseMove(int x, int y);

void drawLine(int x1, int y1, int x2, int y2);

using namespace std;

int main(int argc, char *argv[])
{

    line_thickness = atoi(argv[1]);

    if(glfwInit() == GL_FALSE)
    {
        std::cerr << "failed to init GLFW" << std::endl;
        return 1;
    }

    // select opengl version 
    glfwOpenWindowHint(GLFW_OPENGL_PROFILE, 0);
    glfwOpenWindowHint(GLFW_OPENGL_VERSION_MAJOR, 2);
    glfwOpenWindowHint(GLFW_OPENGL_VERSION_MINOR, 0);

    // create a window
    if(glfwOpenWindow(windowWidth, windowHeight, 0, 0, 0, 8, 24, 8, GLFW_WINDOW) == GL_FALSE)
    {
        std::cerr << "failed to open window" << std::endl;
        glfwTerminate();
        return 1;
    }
    
    
    glfwSetWindowCloseCallback(closedWindow);//set function invoked on window close
    glfwSetWindowSizeCallback(resizeWindow);//set function invoked on window resize
    //glfwSetMouseButtonCallback(mouseClick);
    glfwSetMousePosCallback(mouseMove);

    glDisable(GL_DEPTH_TEST);
    pixel_data = new float[windowWidth * windowHeight];

    //for(int i=0;i<100;i++){pixel_data[i] = 1;}
    //cout << pixel_data[0] << endl;//debug
    // creation and initialization of stuff goes here

    running = true;
    while(running)
    {    
        if(refresh)
        {
            refresh = false;
        glClearColor(0.0f, 1.0f, 1.0f, 1.0f);
               

        // clear first
    glClear(GL_COLOR_BUFFER_BIT //| GL_DEPTH_BUFFER_BIT
        );

        // drawing etc goes here
        // ...

    glDrawPixels(windowWidth,windowHeight,GL_LUMINANCE,GL_FLOAT,pixel_data);

};

 // terminate on escape 
        if(glfwGetKey(GLFW_KEY_ESC))
        {
            running = false;
        }

        // check for errors
    GLenum error = glGetError();
    if(error != GL_NO_ERROR)
    {
        std::cerr << gluErrorString(error);
        running = false;       
    }

        // finally swap buffers
    glfwSwapBuffers();       
}

delete[] pixel_data;
glfwCloseWindow();
glfwTerminate();
return 0;
}//main


void GLFWCALL resizeWindow(int width, int height)
{

  //  windowWidth = width; windowHeight = height;
cerr << "resizeWindow: " << "width:" << width << "\t height:" << height << endl;//debug


glfwSetWindowSize(windowWidth,windowHeight);
}//resizeWindow

/*
void GLFWCALL mouseClick(int button, int action)
{

int *x = new int(0); int *y = new int(0);

glfwGetMousePos(x,y);

if(button == GLFW_MOUSE_BUTTON_LEFT)
{
    pixel_data[xyTO1d(*x,*y)] = 1;
}

cerr << "mouseClick: " << button << "\t x:" << *x << "\t y:" << *y << endl;//debug

delete x;delete y;
}//mouseClick
*/

void GLFWCALL mouseMove(int x, int y)
{

    if(glfwGetMouseButton(GLFW_MOUSE_BUTTON_LEFT))
    {
        //pixel_data[xyTO1d(x,windowHeight-y)] = 1;
refresh = true;
        if((buf_x != -1) && (buf_y !=-1))
            drawLine(buf_x,windowHeight-buf_y,x,windowHeight-y);
        buf_x = x; buf_y = y;

    }
    else
    {
        buf_x = -1;buf_y = -1;
    }


}//mouseMove

void drawLine(int x1, int y1, int x2, int y2)
{
    int length = sqrt((float)((y2 - y1)*(y2 - y1) + (x2 - x1)*(x2 -x1)));



    for(int i=0; i < length;i++)//draw each pixel of line
    {
     int center_x = (x2 - x1)*((float)i/(float)length)+x1;
     int center_y = (y2 - y1)*((float)i/(float)length)+y1;

     pixel_data[xyTO1d(center_x,center_y)] =  1;

     for(int pixel_x = center_x - (line_thickness - 1); pixel_x < center_x + (line_thickness - 1);pixel_x++)
        for(int pixel_y = center_y - (line_thickness - 1); pixel_y < center_y + (line_thickness - 1);pixel_y++)
        {
            int x = pixel_x - center_x;
            int y = pixel_y - center_y;

            if(((x*x)+(y*y))<((line_thickness-1) * (line_thickness-1)))
            {                
                pixel_data[xyTO1d(pixel_x,pixel_y)] =  1;
            }
        }

    }
} 
