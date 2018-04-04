#ifndef _HELPER_H_
#define _HELPER_H_

#include "demo.h"

#define ARRSIZE(arr) (sizeof(arr)/sizeof(*(arr))) //for quicksort

#define FILL(arr, val) \
for(int i_##arr = 0; i_##arr < sizeof arr / sizeof *arr; ++i_##arr) \
{ \
    arr[i_##arr][3] = val;\
}

//-----------------------Mathematical constants
#define PI 3.14159265

//-----------------------Structures that are used 

//for spiral
typedef struct Circle 
{
	double circum[1000][5]; //x,y,heading,radius,altitude
	int len_c; 
	int turns;
}Circle;


typedef struct Packet //used to send initial and start points to new thread and to store a path
{
	double p1[3]; //x,y,heading
	double p2[3];
	double min_rad;
	double start_altitude;     
	double current_path[1000][5]; //x,y,heading,radius,altitude
	int len_cp; 
	int angle;
        //safety metrics
	int num_turns; 
	double avg_alt;
	double min_alt_turn;
	double avg_dist_rnwy;
	double total_dist;
	double time;
	double avg_bank_angle_over_height;
}Packet;

typedef struct Pair //used to hold new points along heading
{
	double x;
 	double y;
	double heading;
}Pair;

typedef struct Seg //to store segments of a dubins path
{
	double C1[10000][5]; //x,y,heading,radius,altitude
	int lenc1;
	double SLS[10000][5];
	int lensls;
	double C2[10000][5];
	int lenc2;	
}Seg;


//-----------------------Helper functions

//for inplace sorting by qsort() in thread function
int compare(const void *a, const void *b);

//finds distance of a point from a line
double pointdist(double a, double b, double c, int x, int y);

//finds the position of a point wrt a line.  
double position(double A, double B, double C, int x, int y);

//finds a new pont along a given heading 
Pair along_heading(double x0, double y0, double theta);

//finds centre of spiral 
Pair circle_centre(double x0, double y0, double theta, double radius);

//returns heading of line between two points in radians
double heading(double x1, double y1, double x2, double y2);

//spits path into segments and returns them.
Seg split(Result dub);

//Mathematical function that calculates Horizontal Distance between two points
double horizontal(double x1, double y1, double x2, double y2);

//Mathematical function that calculates new altitude for straight line motion
double heightS(double last_height, double distance,double baseline_g);

//Mathematical function that calculates new altitude for 30 degree banked turns
double heightBC(double last_height, double distance, int angle,double baseline_g);

//Assigns altitude at each point of DP
Seg assign_altitude(Seg parts, double last_height, double last_x, double last_y, int angle,double baseline_g);

//Prints a path to an new file if found
void print_path(Packet pth, int angle, int heit);

//Fixes altitude difference at end
Seg fix_difference(Seg parts, double diff);

//generates circles for spiral
Circle generate_circle(double x, double y, double heading, double radius, double height, int angle, int modifier,double baseline_g);

#endif
