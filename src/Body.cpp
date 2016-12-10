#include <cstdio>
#include "Body.h"


//comparison functions for body

void body::print(){
	printf("id: %d, x: %f, y: %f, z: %f, Vx: %f, Vy: %f, Vz: %f  mass:%f\n", id, location.axis1, location.axis2, location.axis3, velocity.axis1, velocity.axis2, velocity.axis3, mass);
}

void body::toString(int num_dim, char* output){
	switch(num_dim){
		case 1:
			sprintf(output, "%f\t%f\t%f\t%i\n", location.axis1, velocity.axis1, mass, id);
			break;
		case 2:
			sprintf(output, "%f\t%f\t%f\t%f\t%f\t%i\n", location.axis1, location.axis2, 
					velocity.axis1, velocity.axis2, mass, id);
			break;
		case 3: 

			sprintf(output, "%f\t%f\t%f\t%f\t%f\t%f\t%f\t%i\n", 
					location.axis1, location.axis2, location.axis3,
					velocity.axis1, velocity.axis2, velocity.axis3, mass, id);
			break;
		default:
			sprintf(output, "Invalid Dim");
	}

}

bool body_comp::comp1(body* lhs, body* rhs){
	return lhs->location.axis1 < rhs->location.axis1;
}

bool body_comp::comp2(body* lhs, body* rhs){
	return lhs->location.axis2 < rhs->location.axis2;
}

bool body_comp::comp3(body* lhs, body* rhs){
	return lhs->location.axis3 < rhs->location.axis3;
}
