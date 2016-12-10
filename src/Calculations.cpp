#include "Calculations.h"


double calculate_distance (body &lhs, body &rhs, int num_dim){

	double distance = pow(std::abs(lhs.location.axis1-rhs.location.axis1),2);  
	switch(num_dim){
		case 3:
			distance += pow(std::abs(lhs.location.axis3-rhs.location.axis3),2);
		case 2:
			distance += pow(std::abs(lhs.location.axis2-rhs.location.axis2),2);
	}
	distance = sqrt(distance);
	return distance;
}

double calculate_force (body &lhs, body &rhs, double distance_inverse){
	double force = lhs.mass * rhs.mass * GRAVITATIONAL_CONSTANT * distance_inverse;
	return force;	
}

void calculate_gravitational_force(body &lhs, body &rhs, int num_dim, double time_step){
	double axis1, axis2, axis3;
	double distance, force;
	double distance_inverse;
	
	distance = calculate_distance(lhs, rhs, num_dim);
	distance_inverse = 1/distance;

	force = calculate_force(lhs,rhs, distance_inverse);
	
	//calculate velocity change as a result of that force 
	//directions are from rhs point of view, lhs will be the negative of rhs
	axis1 = lhs.location.axis1-rhs.location.axis1;axis1 *= distance_inverse;
	rhs.velocity.axis1 += axis1 * force * time_step;
	lhs.velocity.axis1 += -axis1 * force * time_step;
	switch (num_dim){
		case 3:
			axis3 = lhs.location.axis3-rhs.location.axis3;
			axis3 *= distance_inverse;
			rhs.velocity.axis3 += axis3 * force * time_step;
			lhs.velocity.axis3 += -axis3 * force * time_step;
		case 2:
			axis2 = lhs.location.axis2-rhs.location.axis2;
			axis2 *= distance_inverse;
			rhs.velocity.axis2 += axis2 * force * time_step;
			lhs.velocity.axis2 += -axis2 * force * time_step;
	}


}