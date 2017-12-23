//header containing the various math operations performed in the code

#ifndef CALCULATIONS_H
#define CALCULATIONS_H

#define GRAVITATIONAL_CONSTANT 6.67408 * 0.00000000001

#include <cmath>
#include <cstdint>
#include "Body.h"

double calculate_distance(const body &lhs, const body &rhs, const uint32_t num_dim);
double calculate_force(const body &lhs, const body &rhs, const double distance_inverse);
void calculate_gravitational_velocity_change(body &lhs, body &rhs, const uint32_t num_dim, const double time_step);


#endif

