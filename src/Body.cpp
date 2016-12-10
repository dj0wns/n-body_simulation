#include <cstdio>
#include "Body.h"


//comparison functions for body

void body::print(int num_dim){
	switch(num_dim){
		case 1:
			printf("id: %d, x: %f, Vx: %f, mass:%f\n", 
					id, location.axis1, velocity.axis1, mass);
			break;
		case 2:
			printf("id: %d, x: %f, y: %f, Vx: %f, Vy: %f, mass:%f\n", 
					id, location.axis1, location.axis2,
					velocity.axis1, velocity.axis2,  mass);
			break;
		case 3: 

			printf("id: %d, x: %f, y: %f, z: %f, Vx: %f, Vy: %f, Vz: %f, mass:%f\n", 
					id, location.axis1, location.axis2, location.axis3, 
					velocity.axis1, velocity.axis2, velocity.axis3, mass);
			break;
		default:
			printf("Invalid Dim");
	}
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

void body::step(int num_dim, double time_step){
	location.axis1 += velocity.axis1*time_step;
	switch (num_dim){
		case 3:
		location.axis3 += velocity.axis3*time_step;
		case 2:
		location.axis2 += velocity.axis2*time_step;
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
