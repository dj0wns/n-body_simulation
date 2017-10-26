#include <cstdio>

#include "Octree.h" 
#include "Calculations.h"
#include "File_IO.h"
#include "Body.h"

//Usage:
//./N-Body_Sim [infile] [outfile] [number of timesteps to simulate] [length of timestep in seconds] [accuracy 0-1]

//Notes:
//May change arguments to follow a more standard procedure


void execute(Octree<body> &octree, int num_dim, int iterations, double time_step);

int main(int argc, char ** argv){
	if(argc < 5){
		printf("Insufficient Argument Number.\nUsage:\n./N-Body_Sim [infile] [outfile] [number of timesteps to simulate] [length of timestep in seconds] [accuracy 0-1]\n");
	 	return 0;
	}

	Octree<body> octree;
	int num_dim;
	int iterations = atoi(argv[3]);
	double time_step = atoi(argv[4]);

	octree.comparison_functions.emplace_back(&(body_comp::comp1));
	octree.comparison_functions.emplace_back(&(body_comp::comp2));
	octree.comparison_functions.emplace_back(&(body_comp::comp3));

	read_infile(argv[1], octree, num_dim);
	
	write_logfile(argv[2], octree, 0, num_dim);
	for(int i = 0; i < iterations; i++){
		execute(octree, num_dim, iterations, time_step);
		write_logfile(argv[2], octree, i+1, num_dim);
	}
	
	octree.print(num_dim);

	write_outfile(argv[2], octree, num_dim);



	return 0;
}

void execute(Octree<body> &octree, int num_dim, int iterations, double time_step){
	//iterate through all unique pairs of items in the set
	for(auto it = octree.object_list.begin(); it < octree.object_list.end(); ++it){
		for(auto it2 = it+1; it2 < octree.object_list.end(); ++it2){
			calculate_gravitational_velocity_change(*it, *it2, num_dim, time_step);
		}
	}
	for(auto it = octree.object_list.begin(); it < octree.object_list.end(); ++it){
		it->step(num_dim, time_step);
	}
}
