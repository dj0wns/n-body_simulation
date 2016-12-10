//header containing the various math operations performed in the code

#ifndef CALCULATIONS_H
#define CALCULATIONS_H

#define GRAVITATIONAL_CONSTANT 6.67 * 0.0000000001

#include <cmath>
#include "Body.h"

double calculate_distance(body &lhs, body &rhs, int num_dim);
double calculate_force(body &lhs, body &rhs, double distance_inverse);
void calculate_gravitational_velocity_change(body &lhs, body &rhs, int num_dim, double time_step);


#endif

