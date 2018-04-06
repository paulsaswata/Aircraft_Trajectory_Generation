
#ifndef _DEMO_H_
#define _DEMO_H_


typedef struct Result //used by demo to pack DP and send to calling thread funtion 
{
	double arr[10000][5]; //10000 + causes segfault
        int len;
}Result;

//DO NOT TOUCH. used by Demo
int printConfiguration(double q[3], double x, void* user_data);

//computes & returns DP as a RESULT struct
Result demo(double q0[], double q1[], double min_radius, double interval);

#endif
