#include "demo.h"
#include "helper.h"
#include <stdio.h>
#include <dirent.h> 
#include <string.h>
#include <stdlib.h>
#include <math.h>
#include <pthread.h>
#include <ctype.h>
#include <stdbool.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <time.h>

//main EnTRY function
Seg basic_path(Packet data)
{
	Packet pack; //for storing complete path
 
        int i, limit,j,k;
        
//unpacking packet
        double q1[3];
	double q2[3];
	double min_radius=data.min_rad;
	double start_altitude=data.start_altitude;
	int angle=data.angle;
	double WIND_VELOCITY=data.windspeed;
	double baseline_g=data.baseline_g;

        for(i=0;i<3;i++)
	{
		q1[i]=data.p1[i];
		q2[i]=data.p2[i];
	}
	
	Result dubins=demo(q1,q2,min_radius, data.interval); //sending configs to demo to generate DP
        //dubins.arr =[long, lat, heading, ?unknown]

	Seg dubin_parts=split(dubins); //sending DP to split into segmentss

	dubin_parts=assign_altitude(dubin_parts, start_altitude, q1[0], q1[1], angle, data.baseline_g);//Send dubin_parts to assign_altitude() to get alti for each point

	//generates possible spiral segment with altitude
	Seg path_with_spiral= generate_spiral(dubin_parts,min_radius,angle,data.baseline_g);

	path_with_spiral= find_extended_runway(path_with_spiral,q2[0],q2[1],q2[2],q1[0],q1[1],q1[2],start_altitude, angle, min_radius, data.interval, data.baseline_g, data.dirty_g); //finds extended runway
	print_trajectory(path_with_spiral, angle, q2[0],q2[1],q2[2]); //saving to file
	
	return path_with_spiral;
}

int main()
{
	
	FILE *myFile;
	myFile = fopen("config.txt", "r");			
	float configArray[10];//reading configuration file into array
	if (myFile == NULL)
	{
		printf("Error Reading File\n");
		exit (0);
	}
	for (int i = 0; i < 11; i++)
	{
		fscanf(myFile, "%f,", &configArray[i] );
	}
	fclose(myFile);
	double q1[3];//Initial x,y heading //CONVERT TO 0,0, East-X
	for (int i = 0; i < 3; i++) 
	{
	        q1[i] = configArray[i];
	}
	double q2[3]; //Runway x,y, heading //CONVERT TO 0,0, East-X
	for (int i =0; i < 3; i++) 
	{
	        q2[i] = configArray[i+3];
	}

        double EMERGENCY_ALTITUDE = configArray[6]/364173.0;// in  
	double BEST_GLIDE_RATIO=configArray[7]; 
	double DIRTY_CONFIG_GLIDE_RATIO=configArray[8];
	double BEST_GLIDING_AIRSPEED= configArray[9]; //in knots
	int BANK_ANGLE= configArray[10];


	double INTERVAL =0.001; //use 0.001 for A320 and 0.0001 for c172. NEED TO CHANGE ARRAY SIZES in helper.h STRUCTS!!!!!!!!!
//------//INTERVAL= interval between points in DUBINS generator. sent to demo()


        int i, filename=0;
        char alphabet='h';
	Packet dat; //creating a packet with constants
	for(i=0;i<3;i++)
	{
		dat.p1[i]=q1[i];

		dat.runway[i]=q2[i];
	}	
        dat.interval= INTERVAL;
        dat.start_altitude=EMERGENCY_ALTITUDE; //initial altitude 	
	dat.airspeed= (BEST_GLIDING_AIRSPEED*1.68781/364173.0);
	dat.baseline_g=BEST_GLIDE_RATIO;
	dat.dirty_g=DIRTY_CONFIG_GLIDE_RATIO;
	dat.file_name=filename;
	dat.alphabet=alphabet;

	
	Packet dat_30; //condition specific variables will be initialized in this packet
	dat_30=dat;
	for(i=0;i<3;i++)
	{
		dat_30.p2[i]=q2[i];
	}
	dat_30.angle=BANK_ANGLE;
	dat_30.min_rad=(BEST_GLIDING_AIRSPEED*BEST_GLIDING_AIRSPEED)/(11.29* tan(dat_30.angle*PI/180))/364173.0; //v^2/(G x tan(bank_angle))

	Seg basic_trajectory=basic_path(dat_30); //get first_dubins

	return 0;
}
