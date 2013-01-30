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
int line_thickness = 5;
int buf_x =-1; int buf_y =-1;
int symmetry = 0;
int cur_view = 0;
int keyboard_reading_state = 0;
bool eraser = false;
bool bound_lock = false;
bool row_filling_confirmation = false;
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
void scan_for_bounds(int,bool);
void fill_row(float *array,int start, int end , bool vertical, float value);
//void check_bounds(int x, int y);
void assign_pixel(int x, int y);
void switch_to_view(int);

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








////////////////////////
////////////////MAIN
//////////////////



int main(int argc, char **argv)
{

	//bounds[2][0] = windowHeight; bounds[2][1] = - 1;
	bounds_x = new bool[windowHeight];
	bounds_y = new bool[windowHeight];
	bounds_z = new bool[windowHeight];
	memset(bounds_x,windowHeight,0);
	memset(bounds_y,windowHeight,0);
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
	
	glfwSetWindowTitle("Protrictorium. Front view");

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
			switch(keyboard_reading_state)
			{

				case 0:
				{
					refresh = true;
					switch((char)character)
					{
						case 'f':
						{
							scan_for_bounds(cur_view,true);
						}

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

						case 'l':
						{
							bound_lock = !bound_lock;
							cout << "Boundary lock " << bound_lock << endl;
							break;
						}

						case 'e':
						{
							eraser = !eraser;
							cout << "Eraser " << eraser << endl;
							break;
						}

						case '1':
						{
							switch_to_view(0);

							break;
						}

						case '2':
						{		
							switch_to_view(1);

							break;
						}

						case '3':
						{
							switch_to_view(2);

							break;
						}

						default: break;
}//switch

}
/*
case 1://await confirmation of row filling in destination viewport on row switch
{
if(character == 'y')
	{row_filling_confirmation = true;}
else
	{row_filling_confirmation = false;}
}
*/
default: break;

}//keyboard_reading_state switch

}


void scan_for_bounds(int cur_view, bool perform_filling)
{
	
		switch(cur_view)
	{
		case 0:
		{
			for (int i = 0; i < windowWidth; i++)
			{				
				int sum = 0;
				int sum2 = 0;
				for(int j = 0; j < windowHeight; j++)
				{
					//cerr << i << " " << j<< endl;//debug
					sum += pixel_data_front[xyTO1d(i,j)];
					sum2 += pixel_data_front[xyTO1d(j,i)];
					bounds_x[i] = true;
				}
				if (!sum && bounds_y[i])
				{
					bounds_y[i] = false;
					if(perform_filling)fill_row(pixel_data_top,i,i,true,0);					
				}
				if(sum && !bounds_y[i])
				{
					bounds_y[i] = true;
					if(perform_filling)fill_row(pixel_data_top,i,i,true,1);
				}
				if (!sum2 && bounds_z[i])
				{
					bounds_z[i] = false;
					if(perform_filling)fill_row(pixel_data_side,i,i,false,0);
				}
				if (sum2 && !bounds_z[i])
				{
					bounds_z[i] = true;
					if(perform_filling)fill_row(pixel_data_side,i,i,false,1);
				}



			}
			break;
		}

		case 1:
		{
			for (int i = 0; i < windowWidth; i++)
			{				
				int sum = 0;
				int sum2 = 0;
				for(int j = 0; j < windowHeight; j++)
				{
					//cerr << i << " " << j<< endl;//debug
					sum += pixel_data_side[xyTO1d(i,j)];
					sum2 += pixel_data_side[xyTO1d(j,i)];
					bounds_y[i] = true;
				}
				if (!sum && bounds_x[i])
				{
					bounds_y[i] = false;
					if(perform_filling)fill_row(pixel_data_top,i,i,false,0);					
				}
				if(sum && !bounds_x[i])
				{
					bounds_y[i] = true;
					if(perform_filling)fill_row(pixel_data_top,i,i,false,1);
				}
				if (!sum2 && bounds_z[i])
				{
					bounds_z[i] = false;
					if(perform_filling)fill_row(pixel_data_front,i,i,false,0);
				}
				if (sum2 && !bounds_z[i])
				{
					bounds_z[i] = true;
					if(perform_filling)fill_row(pixel_data_front,i,i,false,1);
				}

			}
			break;
		}

		case 2:
		{
			for (int i = 0; i < windowWidth; i++)
			{				
				int sum = 0;
				int sum2 = 0;
				for(int j = 0; j < windowHeight; j++)
				{
					//cerr << i << " " << j<< endl;//debug
					sum += pixel_data_top[xyTO1d(i,j)];
					sum2 += pixel_data_top[xyTO1d(j,i)];
					bounds_z[i] = true;
				}
				if (!sum && bounds_y[i])
				{
					bounds_y[i] = false;
					if(perform_filling)fill_row(pixel_data_front,i,i,true,0);					
				}
				if(sum && !bounds_y[i])
				{
					bounds_y[i] = true;
					if(perform_filling)fill_row(pixel_data_front,i,i,true,1);
				}
				if (!sum2 && bounds_x[i])
				{
					bounds_z[i] = false;
					if(perform_filling)fill_row(pixel_data_side,i,i,true,0);
				}
				if (sum2 && !bounds_x[i])
				{
					bounds_z[i] = true;
					if(perform_filling)fill_row(pixel_data_side,i,i,true,1);
				}

			}
			break;
		}

		default: break;

	}
	
}


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

/*
void check_bounds(int x, int y)
{

//X - front --> back
	//Y - right --> left (model-wise)
	//Z - bottom --> top

	
	switch(cur_view)
	{
		case 0:
		{

			if(!bounds_y[x])
			{
				if(!eraser)
				{
				bounds_y[x]=true;
				fill_row(pixel_data_top,x,x,true,1);				
				}
			}
			else
			{
				
				if(eraser)
				{
					bool do_erase = true;
					for(int i = 0; i < windowHeight; i++)
					{				
					//cerr << "loop " << i << " " << pixel_data_front[xyTO1d(i,y)] << endl;//debug		
						if(pixel_data_front[xyTO1d(x,i)] == 1.0f)
							{								
								do_erase = false;break;
							}
					}			

					if(do_erase)
					{
						//cerr << "do_erase"   << endl;//debug
						fill_row(pixel_data_top,x,x,true,0);		
						bounds_y[x]=false;
					}
							
				}
			}
			
			if(!bounds_z[y])
			{
				if(!eraser)
				{
				bounds_z[y]=true;
				fill_row(pixel_data_side,y,y,false,1);				
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
			break;
		}//case 0

		case 1:
		{

			if(!bounds_x[x])
			{
				if(!eraser)
				{
				bounds_x[x]=true;
				fill_row(pixel_data_top,x,x,false,1);				
				}
			}
			else
			{
				
				if(eraser)
				{
					bool do_erase = true;
					for(int i = 0; i < windowHeight; i++)
					{				
					//cerr << "loop " << i << " " << pixel_data_front[xyTO1d(i,y)] << endl;//debug		
						if(pixel_data_side[xyTO1d(x,i)] == 1.0f)
							{								
								do_erase = false;break;
							}
					}			

					if(do_erase)
					{
						//cerr << "do_erase"   << endl;//debug
						fill_row(pixel_data_top,x,x,false,0);		
						bounds_x[x]=false;
					}
							
				}
			}
			
			if(!bounds_z[y])
			{
				if(!eraser)
				{
				bounds_z[y]=true;
				fill_row(pixel_data_front,x,x,false,1);				
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
						if(pixel_data_side[xyTO1d(i,y)] == 1.0f)
							{								
								do_erase = false;break;
							}
					}			

					if(do_erase)
					{
						//cerr << "do_erase"   << endl;//debug
						fill_row(pixel_data_front,x,x,false,0);
						bounds_z[y]=false;
					}
							
				}
			}
			break;
		}//case1

		case 2:
		{

			if(!bounds_y[x])
			{
				if(!eraser)
				{
				bounds_y[x]=true;
				fill_row(pixel_data_front,x,x,true,1);				
				}
			}
			else
			{
				
				if(eraser)
				{
					bool do_erase = true;
					for(int i = 0; i < windowHeight; i++)
					{				
					//cerr << "loop " << i << " " << pixel_data_front[xyTO1d(i,y)] << endl;//debug		
						if(pixel_data_top[xyTO1d(x,i)] == 1.0f)
							{								
								do_erase = false;break;
							}
					}			

					if(do_erase)
					{
						//cerr << "do_erase"   << endl;//debug
						fill_row(pixel_data_front,x,x,true,0);		
						bounds_y[x]=false;
					}
							
				}
			}
			
			if(!bounds_x[y])
			{
				if(!eraser)
				{
				bounds_x[y]=true;
				fill_row(pixel_data_side,y,y,true,1);				
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
						if(pixel_data_top[xyTO1d(i,y)] == 1.0f)
							{								
								do_erase = false;break;
							}
					}			

					if(do_erase)
					{
						//cerr << "do_erase"   << endl;//debug
						fill_row(pixel_data_side,y,y,true,0);
						bounds_x[y]=false;
					}
							
				}
			}
			break;
		}//case2

	default : break;

	}
	

}//check_bounds
*/

void assign_pixel(int x, int y)
{

	float value = !eraser;

	
	//cerr << x<< '\t' << y << '\t' << xyTO1d(x,y) << endl;//debug

	if ( (x < windowWidth) && (x >= 0) && (y >=0) && (y<windowHeight))
	{

		int pixelIndex = xyTO1d(x,y);
		if(cur_view == 0)
		{
			//if(bound_lock)if(!bounds_z[y] || !bounds_y[x])return;
			pixel_data_front[pixelIndex] =  value;
		}

		if(cur_view == 1)
		{
			//if(bound_lock)if(!bounds_x[x] || !bounds_z[y])return;
			pixel_data_side[pixelIndex] =  value;
		}

		if(cur_view == 2)
		{
			//if(bound_lock)if(!bounds_x[y] || !bounds_y[x])return;
			pixel_data_top[pixelIndex] =  value;
		}

		//check_bounds(x,y);
	}

	

}

void switch_to_view(int view)
{

//cout << "Would you like to fill rows? " << flush;
//if(cin.get() == 'y'){row_filling_confirmation = true;};

	//if(row_filling_confirmation)
	//	scan_for_bounds(cur_view,false);
	row_filling_confirmation = false;
	//bound_lock = true;
	cur_view = view;
	switch(view){
		case 0:glfwSetWindowTitle("Protrictorium. Front view");break;
		case 1:glfwSetWindowTitle("Protrictorium. Side view");break;
		case 2:glfwSetWindowTitle("Protrictorium. Top view");break;
		default:break;
	}
}