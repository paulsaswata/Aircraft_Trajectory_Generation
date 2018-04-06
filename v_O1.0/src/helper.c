#include "helper.h"
#include "demo.h"
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <math.h>

#define ARRSIZE(arr) (sizeof(arr)/sizeof(*(arr)))//for quicksort

#define FILL(arr, val) \
for(int i_##arr = 0; i_##arr < sizeof arr / sizeof *arr; ++i_##arr) \
{ \
    arr[i_##arr][3] = val;\
}


static int path_number=1; //for generating automatic incremental filenames

//for inplace sorting in thread function
int compare(const void *a, const void *b)
{
	//a and b are first columns
	double d1 = *(((const double*)a)+3);//value of 4th clolumn
	double d2 = *(((const double*)b)+3);
	if (d1 > d2) return  1;
	if (d1 < d2) return -1;
	return 0;
}

//finds distance of a point from a line
double pointdist(double a, double b, double c, int x, int y)
{
	double numerator= fabs((a*x)+(b*y)+c);
	double denominator= sqrt(a*a+b*b);
	double dist= numerator/denominator;
        return dist;   
}

//finds the position of a point wrt a line.  
double position(double A, double B, double C, double x, double y)
{ 
	/*
	We conclude that the point A(x1,y1) is ABOVE (anticlockwise) the line ax+by+c=0 if
	(i) ax1+by1+c>0 and b>0
	(ii) ax1+by1+c<0 and b<0
	Therefore, multiplication >0

	We conclude that the point A(x1,y1) is BELOW (clockwise) the line ax+by+c=0 if
	(i) ax1+by1+c<0 and b>0
	(ii) ax1+by1+c>0 and b<0
	Therefore, multiplication <0

	*/
	double s = ((A*x) + (B*y) + C) * B;
	return s;
}

//finds a new pont along a given heading 50 feet away
Pair along_heading(double x0, double y0, double theta)
{
	Pair new_point;
//      double rad= theta * PI / 180; //converting to radians if in degrees
        double rad=theta;
	new_point.x= x0 + 0.00014*cos(rad); //50 feet away
	new_point.y= y0 + 0.00014*sin(rad);
        new_point.heading=theta; 
        return new_point;
}

//finds a new pont along a given heading at a given distance
Pair along_heading_at_distance(double x0, double y0, double theta, double distance)
{
	Pair new_point;

        double rad=theta;
	new_point.x= x0 + distance*cos(rad); //distance units away
	new_point.y= y0 + distance*sin(rad);
        new_point.heading=theta; 
        return new_point;
}

//finds centre of turn given starting point x0,y0; heading of starting point theta and radius of turn.   
Pair circle_centre(double x0, double y0, double theta, double radius)
{
	Pair new_point;
    double rad=theta;
	new_point.x= x0 + radius*cos(rad); 
	new_point.y= y0 + radius*sin(rad);
    new_point.heading=theta; 
    return new_point;
}
//returns heading of line between two points in radians
double heading(double x1, double y1, double x2, double y2)
{
	double slope = (y2-y1)/(x2-x1);
	double ret= atan(slope);
	return ret;
	
}

//spits path into segments and returns them.
Seg split(Result dub)
{
	Seg segments;
	int i, j, k, l;
	for (i=0;i<dub.len;i++)
	{
		if(dub.arr[i][2]!=dub.arr[i+1][2])
		{
			segments.C1[i][0]=dub.arr[i][0];
		        segments.C1[i][1]=dub.arr[i][1];
		        segments.C1[i][2]=dub.arr[i][2];
		        segments.C1[i][3]=dub.arr[i][3];
		}
		else 
			break;		
	}
	segments.lenc1=i;
        k=0;	
	for(j=i;j<dub.len;j++)
	{
		if(dub.arr[j][2]==dub.arr[j+1][2])
		{
         		segments.SLS[k][0]=dub.arr[j][0];
	      		segments.SLS[k][1]=dub.arr[j][1];
		        segments.SLS[k][2]=dub.arr[j][2];
		        segments.SLS[k][3]=dub.arr[j][3];
			k++;	
		}
		else 
                { //for last of SLS 
                        segments.SLS[k][0]=dub.arr[j][0];
		        segments.SLS[k][1]=dub.arr[j][1];
		        segments.SLS[k][2]=dub.arr[j][2];
		        segments.SLS[k][3]=dub.arr[j][3];
			k++; 
		    	break;
		} 
	}

	segments.lensls=k;
	i=0;
	k=0;
	for(i=j+1; i<dub.len;i++)
	{
		segments.C2[k][0]=dub.arr[i][0];
		segments.C2[k][1]=dub.arr[i][1];
		segments.C2[k][2]=dub.arr[i][2];
		segments.C2[k][3]=dub.arr[i][3];
		k++;		
	}
        segments.lenc2=k;
        return segments;	
}

//Mathematical function that calculates Horizontal Distance between two points
double horizontal(double x1, double y1, double x2, double y2)
{
	double dist=sqrt((x1 - x2) * (x1 - x2) + (y1 - y2) * (y1 - y2));
        return dist; 
}


//Mathematical function that calculates Horizontal Distance between two points in 3D
double distance_3d(double x1, double y1, double z1, double x2, double y2, double z2)
{
	
	return sqrt((x1-x2)*(x1-x2)+(y1-y2)*(y1-y2)+(z1-z2)*(z1-z2));
}

//Mathematical function that takes in heading WRT E=0 in radian and returns heading WRT N=0 in degrees
double azmth(double E_rad)
{
	double E_deg = E_rad * (180.0/PI);
        double h = 450.0 - E_deg;
        double N_deg=0.0;
        if(h>=360.0)
	{
		N_deg = h - 360.0;
	}
	else
	{
		N_deg = h;
	}
	return N_deg;
}

//Function that finds centre of an arc given three points on that arc
Pair find_centre(double x1, double y1, double x2, double y2, double x3, double y3)
{
	Pair center; // to store the center
	//finding perpendicular bisector pA.X + pB.Y + pC = 0
		//for line 1-----2 let the line be P
	double PA, PB, PC, Pm, midX12, midY12;
	midX12= (x2+x1)/2;
	midY12= (y2+y1)/2;
	Pm= (x1-x2)/(y2-y1);

	PA=Pm;
	PB=-1;
	PC=midY12-(Pm*midX12);  

	//for line 1-----3 let the line be Q
	double QA, QB, QC, Qm, midX13, midY13;
	midX13= (x3+x1)/2;
	midY13= (y3+y1)/2;
	Qm= (x1-x3)/(y3-y1);

	QA=Qm;
	QB=-1;
	QC=midY13-(Qm*midX13);    
	/* Will be needed below
	Cross multiplication method of solving equations
	IF:
	a1x + b1y + c1 =0 ...(1)
	a2x + b2y + c2 =0 ...(2)

	THEN:
	x = (b1*c2 - b2*c1) / (a1*b2 - a2*b1)
	y = (c1*a2 - c2*a1) / (a1*b2 - a2*b1)
	*/
	//finding centre USING CROSS MULTIPLPICATION METHOD 
	center.x= ((PB*QC)-(QB*PC))/((PA*QB)-(QA*PB));
	center.y= ((PC*QA)-(QC*PA))/((PA*QB)-(QA*PB));
	
	return center;
}

//Mathematical function that calculates new altitude
double heightS(double last_height, double distance, double Rg_straight)
{
        double heigt= last_height - (distance/Rg_straight);
        return heigt;  
}

//Mathematical function that calculates new altitude for banked turns
double heightBC(double last_height, double distance, int angle, double Rg_straight)
{	
	double Rg;
	if(angle==20)
		Rg=Rg_straight*cos(20*PI/180);
	if(angle==30)
		Rg=Rg_straight*cos(30*PI/180);
	if(angle==45)
		Rg=Rg_straight*cos(45*PI/180);
        double heigt= last_height - (distance/Rg);
        return heigt;  

}

//Assigns altitude at each point of DP
Seg assign_altitude(Seg parts, double last_height, double last_x, double last_y, int angle, double Rg_straight)
{
	int i;
	double current_height,current_x,current_y; //contains new altitude and last point after each assignment
        current_height=last_height;
        current_x=last_x;
        current_y=last_y;  

        //Altitudes for C1
	for(i=0; i<parts.lenc1;i++) 
	{
        	double distance= horizontal(current_x, current_y,parts.C1[i][0],parts.C1[i][1]);
		parts.C1[i][4]=heightBC(current_height, distance, angle, Rg_straight);
                current_height= parts.C1[i][4]; //updating current
		current_x=parts.C1[i][0];
		current_y=parts.C1[i][1];                   
	}
	
	//Altitudes for SLS
	for(i=0; i<parts.lensls;i++)
	{
        	double distance= horizontal(current_x, current_y,parts.SLS[i][0],parts.SLS[i][1]);
		parts.SLS[i][4]=heightS(current_height, distance, Rg_straight);
                current_height= parts.SLS[i][4]; //updating current
		current_x=parts.SLS[i][0];
		current_y=parts.SLS[i][1];	
	}
	
        //Altitudes for C2
	for(i=0; i<parts.lenc2;i++) 
	{
        	double distance= horizontal(current_x, current_y,parts.C2[i][0],parts.C2[i][1]);
		parts.C2[i][4]=heightBC(current_height, distance, angle, Rg_straight);
                current_height= parts.C2[i][4]; //updating current
		current_x=parts.C2[i][0];
		current_y=parts.C2[i][1];
	}
        return parts;                          
}
//new print function
void print_trajectory(Seg path, int angle, double rnwy_x, double rnwy_y, double rnwy_heading)
{
	int i,n;
	char file_name[20];
	sprintf(file_name, "path.csv"); //generating new file name		
        FILE *output_file, *end_point_file;
    	output_file = fopen(file_name, "w");
	end_point_file=fopen("ends.csv","w"); 
	for(i=0;i<path.lenc1;i++)
	{
		fprintf(output_file, "%f,%f,%f,%f\n", path.C1[i][0],path.C1[i][1],path.C1[i][2],(path.C1[i][4]*364173));
	}	

	for(i=0;i<path.lensls;i++)
	{
		fprintf(output_file, "%f,%f,%f,%f\n", path.SLS[i][0],path.SLS[i][1],path.SLS[i][2],(path.SLS[i][4]*364173));
	}
	fprintf(end_point_file, "%f,%f,%f,%f\n", path.SLS[0][0],path.SLS[0][1],path.SLS[0][2],(path.SLS[0][4]*364173));
	for(i=0;i<path.lenc2;i++)
	{
		fprintf(output_file, "%f,%f,%f,%f\n", path.C2[i][0],path.C2[i][1],path.C2[i][2],(path.C2[i][4]*364173));
	}
	fprintf(end_point_file, "%f,%f,%f,%f\n", path.C2[0][0],path.C2[0][1],path.C2[0][2],(path.C2[0][4]*364173));
	fprintf(end_point_file, "%f,%f,%f,%f\n", path.C2[path.lenc2-1][0],path.C2[path.lenc2-1][1],path.C2[path.lenc2-1][2],(path.C2[path.lenc2-1][4]*364173));
	if(path.lenspiral>0)
	{
	fprintf(end_point_file, "%f,%f,%f,%f\n", path.Spiral[path.lenspiral-1][0],path.Spiral[path.lenspiral-1][1],path.Spiral[path.lenspiral-1][2],(path.Spiral[path.lenspiral-1][4]*364173));

		for(i=0;i<path.lenspiral;i++)
		{
			fprintf(output_file, "%f,%f,%f,%f\n", path.Spiral[i][0],path.Spiral[i][1],path.Spiral[i][2],(path.Spiral[i][4]*364173));
		}
	}
	if(path.extended)
	{
		fprintf(output_file, "%f,%f,%f,%f\n", rnwy_x,rnwy_y,rnwy_heading,0.0000);
	}

	fclose(end_point_file);
	fclose(output_file);
}


//generates circles for spiral
Seg generate_spiral(Seg path, double radius, int angle, double Rg_straight)
{
	path.lenspiral=0;// initializing
	//printf("initialized\n");

	double step= 2*PI/50; //50 points per spiral circle
	//get starting theta from last point of dubins
	double last_x= path.C2[path.lenc2-1][0];
	double last_y= path.C2[path.lenc2-1][1];
	double last_altitude= path.C2[path.lenc2-1][4];
	Pair centre= find_centre(path.C2[path.lenc2-1][0], path.C2[path.lenc2-1][1], path.C2[path.lenc2-2][0], path.C2[path.lenc2-2][1], path.C2[path.lenc2-3][0], path.C2[path.lenc2-3][1]);//centre of turn

	double start_theta= acos((last_x-centre.x)/radius); //angle of starting point wrt centre 

	//generate circles
	int i, integral_turns=0, last_integral_i=0;
	step=-step; //for clockwise turn, make positive for anti clockwise turns	
	
	for(i=0;i<300;i++)
	{
		path.Spiral[i][0]=centre.x+(radius*cos((i*step)-start_theta)); //check - start or + start
		path.Spiral[i][1]=centre.y+(radius*sin((i*step)-start_theta));	
		path.Spiral[i][2]=path.C2[path.lenc2-1][2]+fabs(i*step); //MIGHT NEED TO CHANGE> ROUGH CALCULATION 

		//assign altitude
		double distance= horizontal(last_x,last_y,path.Spiral[i][0],path.Spiral[i][1]);
		path.Spiral[i][4]=heightBC(last_altitude, distance, angle, Rg_straight);

		//DELETING NOT INTEGRAL PART		
		if(path.Spiral[i][4]<=0)
		{
			path.lenspiral=last_integral_i;
			break;
		}				

		if((i) % 51 == 0) //integral turns
		{

			last_integral_i=i;
		}		

		last_x=path.Spiral[i][0];
		last_y=path.Spiral[i][1];
		last_altitude=path.Spiral[i][4];
		path.lenspiral=path.lenspiral+1;
	}
	path.spiral_centre= centre;
	path.spiral_start_angle= start_theta;
		
	return path;
}

//Function to find extended runway segment //MIGHT NEED FIXING
Seg find_extended_runway(Seg path, double rnwy_x, double rnwy_y, double rnwy_heading , double init_x, double init_y, double init_heading, double start_altitude, int angle, double min_radius, double interval, double Rg_straight, double Rg_dirty)
{
	double end_altitude;
	if(path.lenspiral>0)
	{
		 end_altitude=path.Spiral[path.lenspiral-1][4];
	}
	else
	{
		 end_altitude=path.C2[path.lenc2-1][4];
	}
	double q2[]={rnwy_x,rnwy_y,rnwy_heading};	
	double q1[]={init_x,init_y,init_heading};
	if(end_altitude>0.000137) //50 feet
	{
		double current_x=rnwy_x;
		double current_y=rnwy_y;
		double heading=rnwy_heading;
		double heading_opp= heading+PI;
		while(end_altitude>0.000137) //more than 50 feet
		{

	//finding new point at d feet away (using d=50 to find discrete points 50 feet apart)
			Pair new_point=along_heading(current_x, current_y, heading_opp);
			double q3[]={new_point.x,new_point.y,heading};
	//loss of altitude in SL from new to runway
			double distance=horizontal(new_point.x, new_point.y, rnwy_x, rnwy_y);
			double loss=(distance/Rg_dirty);
	//create a path for that point check if end_alti-loss in straight line distance D <50 and >-10		
			Result dubins=demo(q1,q3,min_radius, interval); //sending configs to demo to generate DP
			Seg dubin_parts=split(dubins);//sending DP to split into segments
			//return dubin_parts;
			dubin_parts=assign_altitude(dubin_parts, start_altitude, init_x, init_y, angle, Rg_straight);//Send parts to assign_altitude() to get alti for each point
			Seg path_with_spiral= generate_spiral(dubin_parts,min_radius,angle, Rg_straight);

			int flag=0;
			if(path_with_spiral.lenspiral>0)
			{	flag=1;
				end_altitude= path_with_spiral.Spiral[path_with_spiral.lenspiral-1][4];				
			}
			else 
			{
				end_altitude= path_with_spiral.C2[path_with_spiral.lenc2-1][4];				
			}

			if((end_altitude-loss)<0.000137)
			{
				path_with_spiral.extended=true;
				return path_with_spiral;
			}
			else
			{
				current_x=new_point.x;
				current_y=new_point.y;
			} 	
		}//end of while
	}//end of if
	else
	{
		return path;
	}
}
