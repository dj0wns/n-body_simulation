#include "Calculations.h"

double calculate_distance (const body &lhs, const body &rhs, const uint32_t num_dim){

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

double calculate_force (const body &lhs, const body &rhs, const double distance_inverse_squared){
	double force = lhs.mass * rhs.mass * GRAVITATIONAL_CONSTANT * distance_inverse_squared;
	return force;	
}

void calculate_gravitational_velocity_change(body &lhs, body &rhs, const uint32_t num_dim, const double time_step){
	double axis1, axis2, axis3;
	double distance, force;
	double distance_inverse, distance_inverse_squared;
	distance = calculate_distance(lhs, rhs, num_dim);
	distance_inverse = 1./distance;
	distance_inverse_squared = 1./(distance*distance);

	force = calculate_force(lhs,rhs, distance_inverse_squared);
	//calculate velocity change as a result of that force 
	//directions are from rhs point of view, lhs will be the negative of rhs
	axis1 = lhs.location.axis1-rhs.location.axis1;
  axis1 *= distance_inverse;
	rhs.addedVelocity.axis1 += axis1 * force * time_step / rhs.mass;
	lhs.addedVelocity.axis1 += -axis1 * force * time_step / lhs.mass;
	switch (num_dim){
		case 3:
			axis3 = lhs.location.axis3-rhs.location.axis3;
			axis3 *= distance_inverse;
			rhs.addedVelocity.axis3 += axis3 * force * time_step / rhs.mass; 
			lhs.addedVelocity.axis3 += -axis3 * force * time_step / lhs.mass;
		//falls through
		case 2:
			axis2 = lhs.location.axis2-rhs.location.axis2;
			axis2 *= distance_inverse;
			rhs.addedVelocity.axis2 += axis2 * force * time_step / rhs.mass;
			lhs.addedVelocity.axis2 += -axis2 * force * time_step / lhs.mass;
	}


}
