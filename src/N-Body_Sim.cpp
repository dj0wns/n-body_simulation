#include "Octree.h" 
#include "Calculations.h"
#include "File_IO.h"
#include "Body.h"

//Usage:
//./N-Body_Sim [infile] [outfile] [number of timesteps to simulate] [length of timestep in seconds] [accuracy 0-1]

//Notes:
//May change arguments to follow a more standard procedure

int main(int argc, char ** argv){
	
	Octree<body> octree;
	
	int num_dim;

	octree.comparison_functions.emplace_back(&(body_comp::comp1));
	octree.comparison_functions.emplace_back(&(body_comp::comp2));
	octree.comparison_functions.emplace_back(&(body_comp::comp3));

	read_infile(argv[1], octree, num_dim);
	
	octree.print();

	write_outfile(argv[2], octree, num_dim);

	return 0;
}
