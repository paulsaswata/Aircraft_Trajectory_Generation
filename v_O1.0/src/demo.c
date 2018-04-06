#include "demo.h"
#include "dubins.h"
#include <dirent.h> 
#include <stdio.h> 
#include <string.h>
#include <stdlib.h>
#include <pthread.h>
#include <ctype.h>
#include <stdbool.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>



__thread double array[10000][5]; //Thread Local Storage
__thread int count;             


//DO NOT TOUCH. used by Demo
int printConfiguration(double q[3], double x, void* user_data) 
{ 
	array[count][0]=q[0];    //Successfully storing in array.
	array[count][1]=q[1];
	array[count][2]=q[2];
	array[count][3]=x;	
	count++;
	return 0;
}

//returns DP as a RESULT struct
Result demo(double q0[], double q1[], double min_radius, double interval)
{

//        double interval=0.0001; //INTERVAL OF POINT GENERATION -- REMEMBER TO FIX ARRAY SIZES IN helper.h IF FIDDLING WITH THIS
	DubinsPath path;
	dubins_init( q0, q1, min_radius, &path);
	count=0;	

	dubins_path_sample_many( &path, printConfiguration, interval, NULL);
	Result out; //new RESULT to send output to calling function

	out.len=count;

	int i;
	for(i=0; i<count;i++)
	{
		out.arr[i][0]=array[i][0];
		out.arr[i][1]=array[i][1];
		out.arr[i][2]=array[i][2];
		out.arr[i][3]=array[i][3];
	}
	count=0;     //resetting count for next use
	return out;
}
