#include <cstdio>
#include "Body.h"


//comparison functions for body

void body::print(int num_dim){
  switch(num_dim){
    case 1:
      printf("id: %d, x: %f, Vx: %f, mass: %f, r: %f\n", 
          id, location.axis1, velocity.axis1, mass, radius);
      break;
    case 2:
      printf("id: %d, x: %f, y: %f, Vx: %f, Vy: %f, mass: %f, r: %f\n", 
          id, location.axis1, location.axis2,
          velocity.axis1, velocity.axis2, mass, radius);
      break;
    case 3: 

      printf("id: %d, x: %f, y: %f, z: %f, Vx: %f, Vy: %f, Vz: %f, mass: %f, r: %f\n", 
          id, location.axis1, location.axis2, location.axis3, 
          velocity.axis1, velocity.axis2, velocity.axis3, mass, radius);
      break;
    default:
      printf("Invalid Dim");
  }
}

void body::toString(int num_dim, char* output){
  switch(num_dim){
    case 1:
      sprintf(output, "%f\t%f\t%f\t%f\t%i\n", location.axis1, velocity.axis1, mass, radius, id);
      break;
    case 2:
      sprintf(output, "%f\t%f\t%f\t%f\t%f\t%f\t%i\n", location.axis1, location.axis2, 
          velocity.axis1, velocity.axis2, mass, radius, id);
      break;
    case 3: 

      sprintf(output, "%f\t%f\t%f\t%f\t%f\t%f\t%f\t%f\t%i\n", 
          location.axis1, location.axis2, location.axis3,
          velocity.axis1, velocity.axis2, velocity.axis3, mass, radius, id);
      break;
    default:
      sprintf(output, "Invalid Dim");
  }

}

void body::calculateBoundingBox(int num_dim, double time_step){
  boundingBox.origin.axis1 = location.axis1 - radius *(double)((velocity.axis1 + 0.5*addedVelocity.axis1<0)*-2+1);
  boundingBox.edgeLengths.axis1 = boundingBox.origin.axis1 
      + 2*radius *(double)((velocity.axis1 + 0.5*addedVelocity.axis1<0)*-2+1) 
      + velocity.axis1 * time_step 
      + addedVelocity.axis1 * time_step * 0.5
      - boundingBox.origin.axis1;
  switch (num_dim){
    case 3:
      boundingBox.origin.axis3 = location.axis3 - radius *(double)((velocity.axis3 + 0.5*addedVelocity.axis3<0)*-2+1);
      boundingBox.edgeLengths.axis3 = boundingBox.origin.axis3 
          + 2*radius *(double)((velocity.axis3 + 0.5*addedVelocity.axis3<0)*-2+1) 
          + velocity.axis3 * time_step
          + addedVelocity.axis3 * time_step * 0.5
          - boundingBox.origin.axis3;
    case 2:
      boundingBox.origin.axis2 = location.axis2 - radius *(double)((velocity.axis2 + 0.5*addedVelocity.axis2<0)*-2+1);
      boundingBox.edgeLengths.axis2 = boundingBox.origin.axis2 
          + 2*radius *(double)((velocity.axis2 + 0.5*addedVelocity.axis2<0)*-2+1) 
          + velocity.axis2 * time_step
          + addedVelocity.axis2 * time_step * 0.5
          - boundingBox.origin.axis2;
  }
}

void body::step(int num_dim, double time_step){
  location.axis1 += velocity.axis1*time_step + addedVelocity.axis1 * time_step * 0.5;
  velocity.axis1 += addedVelocity.axis1;
  addedVelocity.axis1=0;
  switch (num_dim){
    case 3:
    location.axis3 += velocity.axis3*time_step + addedVelocity.axis3 * time_step * 0.5;
    velocity.axis3 += addedVelocity.axis3;
    addedVelocity.axis3=0;
    case 2:
    location.axis2 += velocity.axis2*time_step + addedVelocity.axis2 * time_step * 0.5;
    velocity.axis2 += addedVelocity.axis2;
    addedVelocity.axis2=0;
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

bool body_comp::compMiddleBound1(body* lhs, body* rhs){
  return lhs->boundingBox.origin.axis1 + lhs->boundingBox.edgeLengths.axis1*0.5
       < rhs->boundingBox.origin.axis1 + rhs->boundingBox.edgeLengths.axis1*0.5;
}
bool body_comp::compMiddleBound2(body* lhs, body* rhs){
  return lhs->boundingBox.origin.axis2 + lhs->boundingBox.edgeLengths.axis2*0.5
       < rhs->boundingBox.origin.axis2 + rhs->boundingBox.edgeLengths.axis2*0.5;
}
bool body_comp::compMiddleBound3(body* lhs, body* rhs){
  return lhs->boundingBox.origin.axis3 + lhs->boundingBox.edgeLengths.axis3*0.5
       < rhs->boundingBox.origin.axis3 + rhs->boundingBox.edgeLengths.axis3*0.5;
}
