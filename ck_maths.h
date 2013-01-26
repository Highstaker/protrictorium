#include <iostream>
#include <math.h> //Header for Sines and math stuff

float drobnaya(float a)
{
	float b = a - trunc(a);
	return b;
}//drobnaya


float sind(float angle)
{
	return sin(angle * M_PI / 180);
}

float cosd(float angle)
{
	return cos(angle * M_PI / 180);
}

float acosd(float cosine)
{
	float angle = acos(cosine) *180 / M_PI;
	return angle;
}

int array_max(int *a, int size)
{
	int res = a[0];
for(int i = 0; i < size; i++)
	{
if(a[i] > res){res = a[i];}
	}//for

	return res;

}//array_max

int array_min(int *a, int size)
{
	int res = a[0];
for(int i = 0; i < size; i++)
	{
if(a[i] < res){ res = a[i];}
	}//for

	return res;

}//array_min

int array_min_index(int *a, int size)
{
	int res = a[0];
	int res2 = 0;
for(int i = 0; i < size; i++)
	{
if(a[i] < res){res = a[i];res2 = i;}
	}//for

	return res2;

}//array_min_index