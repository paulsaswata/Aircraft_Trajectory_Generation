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


//trajectory generator
Packet func(Packet data, double baseline_g,double dirty_g)
{
	Packet pack; //for storing complete path
        int i, limit,j,k;
        double q1[3];
	double q2[3];
	double min_radius=data.min_rad;
	double start_altitude=data.start_altitude;
	int angle=data.angle;
	int modifier=-1; //to handle weird spirals for LGA22. May need to be used for other runways as well
        for(i=0;i<3;i++)
	{
		q1[i]=data.p1[i];
		q2[i]=data.p2[i];
	}
	printf("----From {%f,%f,%f} \n    to {%f,%f,%f} \n    from %f feet \n    using bank angle %d degrees----\n",q1[0],q1[1],q1[2],q2[0],q2[1],q2[2],(start_altitude*364173.0),angle);
	//check if LGA22 and edit modifier
	int lat,lon,hed;
	lat=q2[0]*10000;
	lon=q2[1]*10000;
	hed=q2[2]*10000;
	if(lat==-738706 && lon==407854 && hed==39444)
	{
		modifier=modifier*(-1);
	}
	//-------------------------------	
	Result fetched=demo(q1,q2,min_radius); //sending configs to demo to generate DP
	Seg parts=split(fetched); //sending DP to split into segments
	parts=assign_altitude(parts, start_altitude, q1[0], q1[1], angle, baseline_g);//Send parts to assign_altitude() to get alti for each point
	Circle circ=generate_circle(parts.C2[parts.lenc2-1][0], parts.C2[parts.lenc2-1][1], parts.C2[parts.lenc2-1][2], min_radius,parts.C2[parts.lenc2-1][4], angle,modifier, baseline_g);
	double end_altitude=circ.circum[circ.len_c-1][4];
	if(end_altitude>0.000137)
		{
			double current_x=q2[0];
			double current_y=q2[1];
			double heading=q2[2];
			double heading_opp= heading+PI;
			while(end_altitude>0.000137) //more than 50 feet
			{
		//finding new point at d feet away (using d=50 to find discrete points 50 feet apart)
				Pair new_point=along_heading(current_x, current_y, heading_opp);
				double q3[]={new_point.x,new_point.y,heading};
		//loss of altitude in SL from new to runway
				double distance=horizontal(new_point.x, new_point.y, q2[0], q2[1]);
				double loss=(distance/dirty_g);
		//create a path for that point check if end_alti-loss in straight line distance D <50 and >-10		
				Result fetched1=demo(q1,q3,min_radius); //sending configs to demo to generate DP
				Seg parts1=split(fetched1); //sending DP to split into segments
				parts1=assign_altitude(parts1, start_altitude, q1[0], q1[1],angle,baseline_g);//Send parts to assign_altitude() to get alti for each point

				Circle circ1=generate_circle(parts1.C2[parts1.lenc2-1][0], parts1.C2[parts1.lenc2-1][1], parts1.C2[parts1.lenc2-1][2], min_radius,parts1.C2[parts1.lenc2-1][4], angle, modifier, baseline_g);
	
				end_altitude= circ1.circum[circ1.len_c-1][4];

				if((end_altitude-loss)<0.000137 && (end_altitude-loss)>0.0)
				{
					parts=parts1;
					circ1.circum[circ1.len_c][0]=q2[0]; //storing final landing point here so that straight line can be drawn
					circ1.circum[circ1.len_c][1]=q2[1];
					circ1.circum[circ1.len_c][2]=q2[2];
					circ1.circum[circ1.len_c][4]=end_altitude-loss;
					circ1.len_c=circ1.len_c+1;	
					circ=circ1;
					break;

				}
				else
				{
					end_altitude=0.1;
					current_x=new_point.x;
					current_y=new_point.y;
				} 	

			}//end of while

		}//end of if
//storing final path in a Packet for printing	
	j=0;		
	double bank_angle_over_height=0.0;
	for(i=0;i<parts.lenc1;i++)
	{
		pack.current_path[j][0]=parts.C1[i][0];
		pack.current_path[j][1]=parts.C1[i][1];
		pack.current_path[j][2]=parts.C1[i][2];
		pack.current_path[j][3]=parts.C1[i][3];
		pack.current_path[j][4]=parts.C1[i][4];
		bank_angle_over_height=bank_angle_over_height+(angle/(parts.C1[i][4]*364173)); 
		j++;	
	}	
	for(i=0;i<parts.lensls;i++)
	{
		pack.current_path[j][0]=parts.SLS[i][0];
		pack.current_path[j][1]=parts.SLS[i][1];
		pack.current_path[j][2]=parts.SLS[i][2];
		pack.current_path[j][3]=parts.SLS[i][3];
		pack.current_path[j][4]=parts.SLS[i][4];
		j++;	
	}
        k=j;
        for(i=0;i<parts.lenc2;i++)
	{
		pack.current_path[j][0]=parts.C2[i][0];
		pack.current_path[j][1]=parts.C2[i][1];
		pack.current_path[j][2]=parts.C2[i][2];
		pack.current_path[j][3]=parts.C2[i][3];
		pack.current_path[j][4]=parts.C2[i][4];
		bank_angle_over_height=bank_angle_over_height+(angle/(parts.C2[i][4]*364173)); 
		j++;	
	}
	for(i=0;i<circ.len_c;i++)
	{
		pack.current_path[j][0]=circ.circum[i][0];
		pack.current_path[j][1]=circ.circum[i][1];
		pack.current_path[j][2]=0.0;
		pack.current_path[j][3]=0.0;
		pack.current_path[j][4]=circ.circum[i][4];
		bank_angle_over_height=bank_angle_over_height+(angle/(circ.circum[i][4]*364173)); 
		j++;	
	}
	pack.len_cp=j;
//remove kink
	pack.current_path[pack.len_cp-2][0]=pack.current_path[pack.len_cp-1][0];
	pack.current_path[pack.len_cp-2][1]=pack.current_path[pack.len_cp-1][1];
	pack.current_path[pack.len_cp-2][2]=pack.current_path[pack.len_cp-1][2];
	pack.current_path[pack.len_cp-2][3]=pack.current_path[pack.len_cp-1][3];
	pack.current_path[pack.len_cp-2][4]=pack.current_path[pack.len_cp-1][4];
//calculate safety
	double total_alt=0.0;
	double total_dist_rnwy=0.0;
	pack.total_dist=0.0;
	pack.num_turns=circ.turns;
	for(i=0;i<pack.len_cp;i++)
	{
		total_alt=total_alt+(pack.current_path[i][4]*364173);
		total_dist_rnwy=total_dist_rnwy+horizontal(pack.current_path[i][0], pack.current_path[i][1], q2[0], q2[1]);
		if(i==0)
			continue;
		else
			pack.total_dist=pack.total_dist+horizontal(pack.current_path[i][0], pack.current_path[i][1], pack.current_path[i-1][0], pack.current_path[i-1][1]);
	}
	pack.avg_alt=total_alt/pack.len_cp;
	pack.avg_dist_rnwy=(total_dist_rnwy/pack.len_cp)*364173;
	pack.total_dist=pack.total_dist*364173;
	pack.avg_bank_angle_over_height=bank_angle_over_height/pack.len_cp;
	pack.time=pack.total_dist/371.318; // 220 kts= 371.318 feet per second
	int heit=start_altitude*364173;
        print_path(pack,angle,heit);
printf("Here are the safety metric values for your trajectory:\n");
printf("average distance from runway=%f\n",pack.avg_dist_rnwy);
printf("average altitude=%f\n",pack.avg_alt);
printf("total distance=%f\n",pack.total_dist);
printf("number of turns=%d\n",pack.num_turns);
printf("average bank angle over height=%f\n",pack.avg_bank_angle_over_height);
printf("time=%f\n",pack.time);
	return pack;
}

int main()
{
	FILE *myFile;
	myFile = fopen("config.txt", "r");			
	float configArray[9];//reading configuration file into array
	if (myFile == NULL)
	{
		printf("Error Reading File\n");
		exit (0);
	}
	for (int i = 0; i < 9; i++)
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
        double start_altitude=configArray[6];// in units
	double baseline_g=configArray[7];
	double dirty_g=configArray[8];
	Packet dat; //packing data for thread
	for(int i=0;i<3;i++)
	{
		dat.p1[i]=q1[i];
		dat.p2[i]=q2[i];
	}	
        dat.start_altitude=start_altitude/364173.0; //initial altitude //CONVERT in term of units.	
	Packet dat30, dat20, dat45;
	dat20=dat;
	dat20.angle=20;
	dat20.min_rad=0.032;
	dat30=dat;
	dat30.angle=30;
	dat30.min_rad=0.02;
	dat45=dat;
	dat45.angle=45;
	dat45.min_rad=0.012;
	Packet p1=func(dat20,baseline_g,dirty_g);
	Packet p2=func(dat30,baseline_g,dirty_g);
	Packet p3=func(dat45,baseline_g,dirty_g);
	return 0;
}

