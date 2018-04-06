# Aircraft_Trajectory_Generation 

The instructions for using the trajectory generation software are given below:

* Create a configuration file with the name `config.txt` in the following format: 
		`initial_longitude,initial_latitude,initial_heading,final_longitude,final_latitude,final_heading,initial_altitude,baseline_glide_ratio,dirty_configuration_glide_ratio,best_gliding_speed,bank_angle`


* For generating trajectories, create a `config.txt` file in the above format, put `generate` and `visualize.py` (for viewing the trajectories in 2D and 3D) in the same folder along with the `config.txt` file and type:


	`$./generate` (To generate trajectories with provided bank angle only)


	`$python visualize.py` (To visualize the generated trajectory)


* The zip file contains a file named `1549_data` that contains the parameters for the runways and the parameters for the various time instances that can be used to recreate the experiments in section VII-B of the paper.


* The `src` folder contains the C source files.

##### Known bugs:
* Static memory allocation is prone to `segmentation fault`
##### Note: 
{dubins.c, dubins.h} - Copyright &copy; 2008-2014, [Andrew Walker](https://github.com/AndrewWalker "Github Link")

##### DISCLAIMER: 
Copyright &copy; 2018 Saswata Paul

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.

#### <p align="middle">***** THIS IS AN OPEN SOURCE PROJECT! *****</p>
