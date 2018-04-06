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
double position(double A, double B, double C, int x, int y)
{ 
	double s = (A*x) + (B*y) + C;
	return s;
}

//finds a new pont along a given heading 
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

//finds centre of spiral 
Pair circle_centre(double x0, double y0, double theta, double radius)
{
	Pair new_point;
//      double rad= theta * PI / 180; //converting to radians if in degrees
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

//Mathematical function that calculates new altitude
double heightS(double last_height, double distance, double baseline_g)
{
        double heigt= last_height - (distance/baseline_g);
        return heigt;  
}

//Mathematical function that calculates new altitude for 30 degree banked turns
double heightBC(double last_height, double distance, int angle, double baseline_g)
{	
	double Rg;
	if(angle==20)
		Rg=baseline_g*cos(20*PI/180);
	if(angle==30)
		Rg=baseline_g*cos(30*PI/180);
	if(angle==45)
		Rg=baseline_g*cos(45*PI/180);
        double heigt= last_height - (distance/Rg);
        return heigt;  

}

//Assigns altitude at each point of DP
Seg assign_altitude(Seg parts, double last_height, double last_x, double last_y, int angle, double baseline_g)
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
		parts.C1[i][4]=heightBC(current_height, distance, angle,baseline_g);
                current_height= parts.C1[i][4]; //updating current
		current_x=parts.C1[i][0];
		current_y=parts.C1[i][1];                   
	}
	
	//Altitudes for SLS
	for(i=0; i<parts.lensls;i++)
	{
        	double distance= horizontal(current_x, current_y,parts.SLS[i][0],parts.SLS[i][1]);
		parts.SLS[i][4]=heightS(current_height, distance,baseline_g);
                current_height= parts.SLS[i][4]; //updating current
		current_x=parts.SLS[i][0];
		current_y=parts.SLS[i][1];	}
	
        //Altitudes for C2
	for(i=0; i<parts.lenc2;i++) 
	{
        	double distance= horizontal(current_x, current_y,parts.C2[i][0],parts.C2[i][1]);
		parts.C2[i][4]=heightBC(current_height, distance, angle, baseline_g);
                current_height= parts.C2[i][4]; //updating current
		current_x=parts.C2[i][0];
		current_y=parts.C2[i][1];
	}

        return parts;                          
}

//Prints a path to an new file if found
void print_path(Packet pth, int angle, int heit)
{	
	int i,n;
	char file_name[20];
	sprintf(file_name, "%d.csv",angle); //generating new file name
	path_number++; //incrememnting the name variable		
        FILE *output_file;
    	output_file = fopen(file_name, "w"); 
//        fprintf(output_file, "X,Y,Heading,Z\n");
	for(i=0;i<pth.len_cp;i++)
	{
                if((pth.current_path[i][4]*364173)<0.0) //display only until above ground 
			break; 
		fprintf(output_file, "%f,%f,%f,%f\n", pth.current_path[i][0],pth.current_path[i][1],pth.current_path[i][2],(pth.current_path[i][4]*364173));

	}	
	fclose(output_file);
}

//generates circles for spiral
Circle generate_circle(double x, double y, double heading, double radius, double altitude, int angle, int modifier,double baseline_g)
{
	Circle circ;
	circ.len_c=0;
	Pair second_point=circle_centre(x,y,heading,radius);	
	double x2=second_point.x;
	double y2=second_point.y;        	 
	double A= (y2 - y);
	double B= (x - x2);
	double C= (x2 * y - x * y2); 
	double slope = (y2-y)/(x2-x);
//finding perpendicular line pA.X + pB.Y + PC = 0 [m2.X - Y + (y-m2.x)] through first pooint (x,y)
			double pA=0, pB=0, pC=0;
			if(A==0)
			{
				pA=1;
				pB=0;
				pC=-x;
			}
			else
			{
				if(B==0)
				{
					pA=0;
					pB=1;
					pC=-y;
				}
				else
				{
					pA=(-1)/slope; 
					pB= -1;
					pC= y-(pA*x);
				}
			}	
	double theta;
	double step=(2*PI/40);
	theta =atan(pA); 
        Pair centre=circle_centre(x,y,theta,radius);
	theta=heading;
	int len=1000;
	int i=0;
	for(i=0;i<len;i++) //generating circle
	{
		circ.circum[i][0]=centre.x + modifier*radius * cos(2*PI-theta+PI/6);
		circ.circum[i][1]=centre.y + modifier*radius * sin(2*PI-theta+PI/6);
		theta=theta+modifier*step;		
	}	
	circ.len_c=len;
	double current_height,current_x,current_y; //contains new altitude and last point after each assignment
        current_height=altitude;
        current_x=x;
        current_y=y;  
	int flag=0;
	circ.turns=-1;
	for(i=0; i<circ.len_c;i++) //assigning altitudes to circles for spiral
	{
        	double distance= horizontal(current_x, current_y,circ.circum[i][0],circ.circum[i][1]);
		circ.circum[i][4]=heightBC(current_height, distance, angle,baseline_g);
		int a,b,c,d;
		a=circ.circum[i][0]*1000000; //converting to int to truncate because after 6 digits accuracy problem
		b=circ.circum[i][1]*1000000;
		c=circ.circum[1][0]*1000000;
		d=circ.circum[1][1]*1000000;
		if(a==c && b==d)
		{
			flag=i;
			circ.turns=circ.turns+1;
		
		}
		if(circ.circum[i][4]<=0)
		{
			circ.len_c=flag;	
			break;
		}	
                current_height= circ.circum[i][4]; //updating current
		current_x=circ.circum[i][0];
		current_y=circ.circum[i][1];
	}
	return circ;
}

