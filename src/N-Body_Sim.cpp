#include "Octree.h" 
#include "Calculations.h"
#include "File_IO.h"
#include "Body.h"

//usage ./N-Body_Sim [path to input]

int main(int argc, char ** argv){
	
	Octree<body> octree;
	
	int num_dims;

	octree.comparison_functions.emplace_back(&(body_comp::comp1));
	octree.comparison_functions.emplace_back(&(body_comp::comp2));
	octree.comparison_functions.emplace_back(&(body_comp::comp3));

	read_infile(argv[1], octree, num_dims);
	
	octree.print();


	return 0;
}
