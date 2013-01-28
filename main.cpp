#include <GL/glfw.h>

#include <iostream>
#include <string.h>
#include "ck_maths.h"
#include <string>
#include <stdlib.h>
//#include <algorithm>  

using namespace std;
//////////////////////
////GLOBALS
///////////////////

bool running;
bool refresh = false;//make this true every time the refresh of screen is required
int windowWidth = 500 ;
int windowHeight = windowWidth;
float *pixel_data_front;
float *pixel_data_side;
float *pixel_data_top;
bool *bounds_x;
bool *bounds_y;
bool *bounds_z;
int line_thickness = 1;
int buf_x =-1; int buf_y =-1;
int symmetry = 0;
int cur_view = 0;
bool eraser = false;
//int bounds[3][2];

/////////////////
////FUNCTION PREDEFINITIONS
//////////////////
void GLFWCALL resizeWindow(int width, int height);
void GLFWCALL mouseClick(int button ,int action);
void GLFWCALL mouseMove(int x, int y);
void GLFWCALL mouseWheel(int pos);
void GLFWCALL keyPress( int character, int action );

void drawLine(int x1, int y1, int x2, int y2);
//void scan_for_bounds();
void fill_row(float *array,int start, int end , bool vertical, float value);
void check_bounds(int x, int y);

//////////////////////
/////////////FUNCTIONS
/////////////////////

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

void assign_pixel(int x, int y)
{

	float value = !eraser;

	
	//cerr << x<< '\t' << y << '\t' << xyTO1d(x,y) << endl;//debug
	if ( (x < windowWidth) && (x >= 0) && (y >=0) && (y<windowHeight))
	{

		int pixelIndex = xyTO1d(x,y);
		if(cur_view == 0)
		{
			pixel_data_front[pixelIndex] =  value;
		}

		if(cur_view == 1)
		{
			pixel_data_side[pixelIndex] =  value;
		}

		if(cur_view == 2)
		{
			pixel_data_top[pixelIndex] =  value;
		}

		check_bounds(x,y);
	}

	

}






////////////////////////
////////////////MAIN
//////////////////



int main(int argc, char **argv)
{

	//bounds[2][0] = windowHeight; bounds[2][1] = - 1;
	 bounds_z = new bool[windowHeight];
	 memset(bounds_z,windowHeight,0);

	cerr << line_thickness << endl;//debug
	for(int i=0;i<argc ;i++)//reading options
	{
		if((string)argv[i] == "-line_thickness")
		{
			line_thickness = atoi(argv[i+1]);
		}
	}

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
	glfwSetMouseButtonCallback(mouseClick);
	glfwSetCharCallback(keyPress);
	glfwSetMousePosCallback(mouseMove);
	glfwSetMouseWheelCallback(mouseWheel);


	glDisable(GL_DEPTH_TEST);

	pixel_data_front = new float[windowWidth * windowHeight];
	pixel_data_side = new float[windowWidth * windowHeight];
	pixel_data_top = new float[windowWidth * windowHeight];
	

	running = true;
	while(running)
	{    
		if(refresh)
		{
			refresh = false;
			glClearColor(0.0f, 1.0f, 1.0f, 1.0f);


			// clear first
			glClear(GL_COLOR_BUFFER_BIT);

			// drawing etc goes here
			// ...
			if(cur_view == 0)
			{
				glDrawPixels(windowWidth,windowHeight,GL_LUMINANCE,GL_FLOAT,pixel_data_front);
			}
			if(cur_view == 1)
			{
				glDrawPixels(windowWidth,windowHeight,GL_LUMINANCE,GL_FLOAT,pixel_data_side);
			}
			if(cur_view == 2)
			{
				glDrawPixels(windowWidth,windowHeight,GL_LUMINANCE,GL_FLOAT,pixel_data_top);
			}

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

	delete[] pixel_data_front;
	delete[] pixel_data_side;
	delete[] pixel_data_top;
	glfwCloseWindow();
	glfwTerminate();
	return 0;
}//main


///////////////////////
////////MORE FUNCTIONS
/////////////////////

void GLFWCALL resizeWindow(int width, int height)
{

	//  windowWidth = width; windowHeight = height;
	cerr << "resizeWindow: " << "width:" << width << "\t height:" << height << endl;//debug


	glfwSetWindowSize(windowWidth,windowHeight);
}//resizeWindow


void GLFWCALL mouseClick(int button, int action)
{

	refresh = true;
	if((button == GLFW_MOUSE_BUTTON_LEFT) && (action == GLFW_PRESS))
	{
		int *x = new int;
		int *y = new int;
		glfwGetMousePos(x,y);
		drawLine(*x,windowHeight-*y,*x,windowHeight-*y);
	}

}//mouseClick


void GLFWCALL mouseMove(int x, int y)
{
	refresh = true;
	if(glfwGetMouseButton(GLFW_MOUSE_BUTTON_LEFT))
	{        

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


	if(!length)length++;

	
	
		for(int i=0; i < length;i++)//draw each pixel of line
		{
			int center_x = (x2 - x1)*((float)i/(float)length)+x1;
			int center_y = (y2 - y1)*((float)i/(float)length)+y1;

			if(line_thickness == 1)
			{
				assign_pixel(center_x,center_y);
				if(symmetry & 2)
					assign_pixel(windowWidth - center_x,center_y);
				if(symmetry & 4)
					assign_pixel(center_x,windowHeight - center_y);
				if(symmetry == 6)
					assign_pixel(windowWidth - center_x,windowHeight - center_y);
			}
			else
			{
				for(int pixel_x = center_x - (line_thickness - 1); pixel_x < center_x + (line_thickness - 1);pixel_x++)
					for(int pixel_y = center_y - (line_thickness - 1); pixel_y < center_y + (line_thickness - 1);pixel_y++)
					{
						int x = pixel_x - center_x;
						int y = pixel_y - center_y;

						if(((x*x)+(y*y))<((line_thickness-1) * (line_thickness-1)))
						{                
							assign_pixel(pixel_x,pixel_y);
							if(symmetry & 2)
								assign_pixel(windowWidth - pixel_x,pixel_y);
							if(symmetry & 4)
								assign_pixel(pixel_x,windowHeight - pixel_y);
							if(symmetry == 6)
								assign_pixel(windowWidth - pixel_x,windowHeight - pixel_y);
						}
					}
				}
			}
		} 


		void GLFWCALL mouseWheel(int pos)
		{
			if(pos > 0){line_thickness+=2;}
			if(pos < 0){line_thickness-=2;}
			if(line_thickness < 0){line_thickness = 1;}

	//cerr << pos << "\t" << line_thickness << endl;//debug
			glfwSetMouseWheel(0);
		}

		void GLFWCALL keyPress( int character, int action )
		{
			refresh = true;
			switch((char)character)
			{
				case 'x':
				{
					symmetry = (symmetry ^ 2);
					break;
				}

				case 'y':
				{
					symmetry = (symmetry ^ 4);
					break;
				}

				case 'e':
				{
					eraser = !eraser;
					break;
				}

				case '1':
				{
					cur_view = 0;

					break;
				}

				case '2':
				{		
					cur_view = 1;
		//void scan_for_bounds();
					break;
				}

				case '3':
				{
					cur_view = 2;
					break;
				}

				default: break;
}//switch

}

/*
void scan_for_bounds()
{
	for(int i = 0; i< windowWidth; i++)
		for(int j = 0; j < windowHeight; j++)
		{
			if(pixel_data_front[xyTO1d(i,j)])
			{
				fill_row(pixel_data_side,bounds[0][0],i,0,1);
				bounds[0][0] = i;
				break;break;
			}
		}

	for(int i = windowWidth; i>=0; i--)
		for(int j = 0; j < windowHeight; j++)
			if(pixel_data_front[xyTO1d(i,j)])
			{
				fill_row(pixel_data_side,bounds[0][1],i,0,1);
				bounds[0][1] = i;
				break;break;
			}
}
*/

void fill_row(float *array,int start, int end , bool vertical, float value)
{

	//cerr << start << '\t' << end << endl;

	if(!vertical)
	{
		
		for(int i = start; i <=end; i++ )
			for(int j = 0; j< windowWidth; j++)
				array[xyTO1d(j,i)] = value;
				
			//memset(&array[start],(int)value,(end - start));
		}
		else
		{
			for(int i = start; i <=end; i++ )
			for(int j = 0; j< windowHeight; j++)
				array[xyTO1d(i,j)] = value;
		}



	}

void check_bounds(int x, int y)
{

//X - front --> back
	//Y - right --> left (model-wise)
	//Z - bottom --> top

	
	switch(cur_view)
	{
		case 0:
		{
			
			if(!bounds_z[y])
			{
				if(!eraser)
				{
				bounds_z[y]=true;
				fill_row(pixel_data_side,y,y,false,1);
				fill_row(pixel_data_top,y,y,true,1);
				}
			}
			else
			{
				
				if(eraser)
				{
					bool do_erase = true;
					for(int i = 0; i < windowWidth; i++)
					{				
					//cerr << "loop " << i << " " << pixel_data_front[xyTO1d(i,y)] << endl;//debug		
						if(pixel_data_front[xyTO1d(i,y)] == 1.0f)
							{								
								do_erase = false;break;
							}
					}			

					if(do_erase)
					{
						//cerr << "do_erase"   << endl;//debug
						fill_row(pixel_data_side,y,y,false,0);
						bounds_z[y]=false;
					}
							
				}
			}

		}

	default : break;

	}
	

}//check_bounds