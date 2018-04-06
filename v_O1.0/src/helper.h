#ifndef _HELPER_H_
#define _HELPER_H_

#include "demo.h"
#include <stdbool.h>

#define ARRSIZE(arr) (sizeof(arr)/sizeof(*(arr))) //for quicksort

#define FILL(arr, val) \
for(int i_##arr = 0; i_##arr < sizeof arr / sizeof *arr; ++i_##arr) \
{ \
    arr[i_##arr][3] = val;\
}

//-----------------------Mathematical constants
#define PI 3.14159265

#define OMEGA_30_DEGREE_BANK 0.2832 //rate of change of heading in standard rate turn for 30 degree bank angle =0.2832 rad/second expressed in units


typedef struct Pair //used to hold new points along heading
{
	double x;
 	double y;
	double heading;
}Pair;

//for any curve
typedef struct Curve 
{
	double points[1000][5]; //x,y,heading,radius,altitude
	int len_curve; 
	int turns;

	bool spiral;
	bool extended;
	Pair centre;

	double shift; //stores the shift of end where needed
	char instructions[1000];
}Curve;

//collection of curves
typedef struct Seg2 
{
	Curve aug_C1;
        Curve aug_SLS;
        Curve aug_C2;
	Curve aug_SPIRAL;
	Curve aug_EXTENDED;
        bool spiral;
        bool extended;
	double total_shift;
	double end_alt;
}Seg2;

//for spiral
typedef struct Circle 
{
	double circum[1000][5]; //x,y,heading,radius,altitude
	int len_c; 
	int turns;
}Circle;

//for discrete thetas
typedef struct Thetas 
{
	double thetas[500]; //an angle
}Thetas;


typedef struct Packet //used to send initial and start points to new thread and to store a path
{	
	double interval; //interval of points in Dubins Path. SENT TO demo()!!
	double p1[3]; //x,y,heading initial config
	double p2[3]; //final config for each call
	double runway[3]; //config of runway
	double current_path[8000][5]; //x,y,heading,radius,altitude
	int len_cp; 

	//emergency variables
	int angle;
	double min_rad;
	double start_altitude;     
	double airspeed;
	double windspeed;
	double wind_heading;
	double baseline_g;
	double dirty_g;

	//misc
	int file_name;
        char alphabet;

}Packet;



typedef struct Seg //to store segments of a dubins path
{
	double C1[1000][5]; //x,y,heading,radius,altitude
	int lenc1;
	double SLS[1000][5];
	int lensls;
	double C2[1000][5];
	int lenc2;	
	double Spiral[1000][5];
	int lenspiral;
	Pair spiral_centre;
	double spiral_start_angle;

	bool extended;
}Seg;


//-----------------------Helper functions

//for inplace sorting by qsort() in thread function
int compare(const void *a, const void *b);

//finds distance of a point from a line
double pointdist(double a, double b, double c, int x, int y);

//finds the position of a point wrt a line.  
double position(double A, double B, double C, double x, double y);

//finds a new pont along a given heading 
Pair along_heading(double x0, double y0, double theta);

//finds a new pont along a given heading at a given distance
Pair along_heading_at_distance(double x0, double y0, double theta, double distance);

//finds centre of turn given starting point x0,y0; heading of starting point theta and radius of turn.   
Pair circle_centre(double x0, double y0, double theta, double radius);

//returns heading of line between two points in radians
double heading(double x1, double y1, double x2, double y2);

//spits path into segments and returns them.
Seg split(Result dub);

//Mathematical function that calculates Horizontal Distance between two points
double horizontal(double x1, double y1, double x2, double y2);

//Mathematical function that calculates Horizontal Distance between two points in 3D
double distance_3d(double x1, double y1, double z1, double x2, double y2, double z2);

//Mathematical function that takes in heading WRT E=0 in radian and returns heading WRT N=0 in degrees
double azmth(double E_rad);

//Function that finds centre of an arc given three points on that arc
Pair find_centre(double x1, double y1, double x2, double y2, double x3, double y3);

//Mathematical function that calculates new altitude for straight line motion
double heightS(double last_height, double distance, double Rg_straight);

//Mathematical function that calculates new altitude for 30 degree banked turns
double heightBC(double last_height, double distance, int angle, double Rg_straight);

//Assigns altitude at each point of DP
Seg assign_altitude(Seg parts, double last_height, double last_x, double last_y, int angle, double Rg_straight);

//new print function
void print_trajectory(Seg path, int angle, double rnwy_x, double rnwy_y, double rnwy_heading);

//Fixes altitude difference at end
Seg fix_difference(Seg parts, double diff);

//generates circles for spiral
Seg generate_spiral(Seg path, double radius, int angle, double Rg_straight);

//Function to find extended runway segment
Seg find_extended_runway(Seg path, double rnwy_x, double rnwy_y, double rnwy_heading, double init_x, double init_y, double init_heading, double init_altitude , int angle, double min_radius, double interval, double Rg_straight,double Rg_dirty);

#endif
