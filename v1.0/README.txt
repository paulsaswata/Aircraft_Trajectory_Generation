***'src' contains the source files

***Format of config.txt file:

initial_latitude,initial_longitude,initial_heading,final_latitude,final_longitude,final_heading,initial_altitude,baseline_glide_ratio,dirty_configuration_glide_ratio

***Instructions:

#To generate trajectories, create a config.txt file in the above format, put 'generate' and 'visualize.py' (for viewing the trajectories in 2D and 3D) in a folder along with the config.txt file and type: 

1. To generate trajectories with 20, 30 and 45 degree bank angles --> $./generate      
2. To visualize generated trajectories --> $python visualize.py

--------------------------------------------------------------------------------------------------------------------------------------

***Known bugs: 

1. The spirals for the high altitude trajectories come out wrong in certain cases. This usually has to be manually dealt with in the function generate_circles() in helper.c where the angle needs to be adjusted for each specific airport. Once fixed, however, all trajectories to that airport can be generated smoothly.

--------------------------------------------------------------------------------------------------------------------------------------
***dubins.c,dubins.h- Copyright (c) 2008-2014, Andrew Walker
