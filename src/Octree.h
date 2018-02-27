//header containing Octree structure and related operations

#ifndef OCTREE_H
#define OCTREE_H

#include <vector>
#include <cstdio>
#include <cmath>
#include <cctype>
#include "Body.h"

#include <algorithm>


template <typename T> class Octree  {
public:
	//use emplace to add objects to list - avoid copy 
	T *object_list; //contains list of all objects - shared among all instances of Octree
	std::vector<T*> leaves; //should be empty unless a leaf node

	//default constructor
	Octree() : leaf_node(false) {};
	
  //default constructor
	Octree( T *object_list) : object_list(object_list), leaf_node(false) {};

  //print stats about tree
	void print(int iteration){
		printf("At iteration %d, the octree has a depth of %d and contains %d leaves. Most leaves in a single node: %d\n", 
        iteration, maxDepth(), totalLeaves(), maxLeaves());
	}

  int totalLeaves(){
    int total = 0;
    getTotalLeaves(total);
    return total;
  }
  
  int maxLeaves(){
    int max = 0;
    getMaxLeaves(max);
    return max;
  }

  int maxDepth(){
    int max = 0;
    getMaxDepth(0,max);
    return max;
  }

  ~Octree(){
    for ( auto it = children.begin(); it < children.end(); ++it){
      delete *it;
    }
  }
 
  //recursively build the tree by subdividing the space
  void create(uint32_t num_dim){
    //calculate average  of bounding box middle for split - should be good enough
    int count = 0;
    //if too few objects not worth splitting
    if(leaves.size() <= (uint32_t)2<<(num_dim-1)){
      leaf_node = true;
      return;
    }
    //create children
    children.reserve(2<<(num_dim-1));
    for (int i = 0 ; i < 2<<(num_dim-1); i++){
      Octree<T>* tempOct = new Octree<T>(object_list);
      children.emplace_back(tempOct);
    }
    for ( auto it = leaves.begin(); it < leaves.end(); ++it){
      division.axis1+=(*it)->boundingBox.origin.axis1 + (*it)->boundingBox.edgeLengths.axis1 * 0.5;
      switch (num_dim){
        case 3:
          division.axis3+=(*it)->boundingBox.origin.axis3 + (*it)->boundingBox.edgeLengths.axis3 * 0.5;
        case 2:
          division.axis2+=(*it)->boundingBox.origin.axis2 + (*it)->boundingBox.edgeLengths.axis2 * 0.5;
      }
    }
    division.axis1/=leaves.size();
    switch (num_dim){
      case 3:
        division.axis3/=leaves.size();
      case 2:
        division.axis2/=leaves.size();
    }
    for ( auto it = leaves.begin(); it < leaves.end(); ++it){
      //for each corner in the bounding box
      switch (num_dim){
        case 3:
          for (int i = 0; i < 8; i++){
            int axis1= (*it)->boundingBox.origin.axis1 + (*it)->boundingBox.edgeLengths.axis1*(i%2);
            int axis2= (*it)->boundingBox.origin.axis2 + (*it)->boundingBox.edgeLengths.axis2*((i>>1)%2);
            int axis3= (*it)->boundingBox.origin.axis3 + (*it)->boundingBox.edgeLengths.axis3*((i>>2)%2);
            std::vector<T*> tempLeaves = children[(axis1>division.axis1)*1 
                + (axis2>division.axis2)*2 
                + (axis3>division.axis3)*4]->leaves;
            if (std::find(tempLeaves.begin(), tempLeaves.end(), *it) ==  tempLeaves.end()){
              tempLeaves.emplace_back(*it);
              count++;
            }
          }
        break;
        case 2:
          for (int i = 0; i < 4; i++){
            int axis1= (*it)->boundingBox.origin.axis1 + (*it)->boundingBox.edgeLengths.axis1*(i%2);
            int axis2= (*it)->boundingBox.origin.axis2 + (*it)->boundingBox.edgeLengths.axis2*((i>>1)%2);
            std::vector<T*> *tempLeaves = &(children[(axis1>division.axis1)*1 + (axis2>division.axis2)*2]->leaves);
            if (std::find(tempLeaves->begin(), tempLeaves->end(), *it) ==  tempLeaves->end()){
              tempLeaves->emplace_back(*it);
              count++;
            }
          }
        break;
      }
    }
    //if children increases number of leaves by 50% or more its not worth splitting
    if (count >= 1.5 * leaves.size())
    {
      leaf_node = true;
      //TODO delete children
      return;

    }
    for ( auto it = children.begin(); it < children.end(); ++it){
      if ( (*it)->leaves.size() >= leaves.size() ){
        leaf_node = true;
        return;
      }
    }

    //leaves.clear();
    for ( auto it = children.begin(); it < children.end(); ++it){
      (*it)->create(num_dim);
    }
    
  }
  
  //Use quadratic formula to solve for the time at with the distance between lhs and rhs is equal to the sum of their radii
  bool testCollision(T &lhs, T &rhs, uint32_t num_dim, double timestep){
    double a=0, b=0, c=0;
    double first=0, second=0;

    switch(num_dim){
      case 3:
        //printf("3D Collision not yet implemented!\n");
        break;
      case 2:
        a = ((lhs.velocity.axis1 + 0.5 * lhs.addedVelocity.axis1) * (lhs.velocity.axis1 + 0.5 * lhs.addedVelocity.axis1))
            + ((rhs.velocity.axis1 + 0.5 * rhs.addedVelocity.axis1) * (rhs.velocity.axis1 + 0.5 * rhs.addedVelocity.axis1))
            + ((lhs.velocity.axis2 + 0.5 * lhs.addedVelocity.axis2) * (lhs.velocity.axis2 + 0.5 * lhs.addedVelocity.axis2))
            + ((rhs.velocity.axis2 + 0.5 * rhs.addedVelocity.axis2) * (rhs.velocity.axis2 + 0.5 * rhs.addedVelocity.axis2))
            - 2 * ((lhs.velocity.axis1 + 0.5 * lhs.addedVelocity.axis1) * (rhs.velocity.axis1 + 0.5 * rhs.addedVelocity.axis1))
            - 2 * ((lhs.velocity.axis2 + 0.5 * lhs.addedVelocity.axis2) * (rhs.velocity.axis2 + 0.5 * rhs.addedVelocity.axis2));
        b = 2 * (lhs.location.axis1 * (lhs.velocity.axis1 + 0.5 * lhs.addedVelocity.axis1))
            + 2 * (rhs.location.axis1 * (rhs.velocity.axis1 + 0.5 * rhs.addedVelocity.axis1))
            + 2 * (lhs.location.axis2 * (lhs.velocity.axis2 + 0.5 * lhs.addedVelocity.axis2))
            + 2 * (rhs.location.axis2 * (rhs.velocity.axis2 + 0.5 * rhs.addedVelocity.axis2))
            - 2 * (lhs.location.axis1 * (rhs.velocity.axis1 + 0.5 * rhs.addedVelocity.axis1))
            - 2 * (rhs.location.axis1 * (lhs.velocity.axis1 + 0.5 * lhs.addedVelocity.axis1))
            - 2 * (lhs.location.axis2 * (rhs.velocity.axis2 + 0.5 * rhs.addedVelocity.axis2))
            - 2 * (rhs.location.axis2 * (lhs.velocity.axis2 + 0.5 * lhs.addedVelocity.axis2));
        c = (lhs.location.axis1 * lhs.location.axis1)
            + (rhs.location.axis1 * rhs.location.axis1)
            + (lhs.location.axis2 * lhs.location.axis2)
            + (rhs.location.axis2 * rhs.location.axis2)
            - 2 * (lhs.location.axis1 * rhs.location.axis1)
            - 2 * (lhs.location.axis2 * rhs.location.axis2)
            - (lhs.radius * lhs.radius)
            - 2 * (lhs.radius * rhs.radius)
            - (rhs.radius * rhs.radius);
        break;
    }
    //verify math is legal
    double tempSQ = (b*b) - 4 * a * c;
    if (tempSQ < 0) {
      //no collision found, sqrt is imaginary
      return false;
    }
    double tempDenom = 2 * a;
    if (tempDenom == 0) {
      //no collision found, divide by 0
      return false;
    }

    //optimization
    tempDenom = 1/tempDenom;

    //quadratic formula
    first = (-b + sqrt( tempSQ )) * tempDenom;
    second = (-b - sqrt( tempSQ )) * tempDenom;
    
    if (first > 0 && second > 0){
      if ( first <= second ) {
        if (first < timestep){
          //printf("Collision found between %d and %d at t = %f\n", lhs.id, rhs.id, first );
        } else {
          //no collision found, first outside of timestep
          return false;
        }
      } else {
        if (second < timestep){
          //printf("Collision found between %d and %d at t = %f\n", lhs.id, rhs.id, second );
        } else {
          //no collision found, second outside of timestep
          return false;
        }
      }

    } else if (first > 0){
        if (first < timestep){
          //printf("Collision found between %d and %d at t = %f\n", lhs.id, rhs.id, first );
        } else {
          //no collision found, first outside of timestep
          return false;
        }
    } else if (second > 0){
        if (second < timestep){
          //printf("Collision found between %d and %d at t = %f\n", lhs.id, rhs.id, second );
        } else {
          //no collision found, second outside of timestep
          return false;
        }
    } else {
      //no collision found, both values negative
      return false;
    }

    //verify by testing distance in distance formula
    double distance_formula_first, distance_formula_second;
    switch (num_dim){
      case 3:
        //printf("3D Collision not yet implemented!\n");
        break;
      case 2:
        distance_formula_first = pow(
                  lhs.location.axis1 + (lhs.velocity.axis1 + 0.5 * lhs.addedVelocity.axis1) * first 
                  - rhs.location.axis1 
                  - (rhs.velocity.axis1 + 0.5 * rhs.addedVelocity.axis1) * first, 2.)
                + pow(
                  lhs.location.axis2 
                  + lhs.velocity.axis2 * first 
                  - rhs.location.axis2 
                  - (rhs.velocity.axis2 + 0.5 * rhs.addedVelocity.axis2) * first, 2.);
        distance_formula_second = pow(lhs.location.axis1 + (lhs.velocity.axis1 + 0.5 * lhs.addedVelocity.axis1) * second 
                  - rhs.location.axis1 - (rhs.velocity.axis1 + 0.5 * rhs.addedVelocity.axis1) * second, 2.)
                  + pow(lhs.location.axis2 
                  + (lhs.velocity.axis2 + 0.5 * lhs.addedVelocity.axis2) * second 
                  - rhs.location.axis2 
                  - (rhs.velocity.axis2 + 0.5 * rhs.addedVelocity.axis2) * second, 2.);
        distance_formula_first = sqrt(distance_formula_first);
        distance_formula_second = sqrt(distance_formula_second);
//        printf("Radius1 = %f; Radius2 = %f; Sum = %f\n", lhs.radius, rhs.radius, lhs.radius + rhs.radius);
//        printf("Result of distance formula: first = %f, second = %f\n",distance_formula_first, distance_formula_second);
//        printf("Collision Time: first = %f, second = %f\n", first, second);
        break;
    }

    return true;
  }

  void checkCollisions(int iterations, uint32_t num_dim, double timestep){
    if(leaf_node){
      for ( auto it = leaves.begin(); it < leaves.end(); ++it){
        for ( auto it2 = it+1; it2 < leaves.end(); ++it2){
          //if either object is deleted dont bother detecting collision
          if ((*it)->toDelete || (*it2)->toDelete){
            continue;
          }
          switch (num_dim){
          case 3:
            if(!(
              (*it)->boundingBox.origin.axis1 + (*it)->boundingBox.edgeLengths.axis1 
                  < (*it2)->boundingBox.origin.axis1
              || (*it2)->boundingBox.origin.axis1 + (*it2)->boundingBox.edgeLengths.axis1 
                  < (*it)->boundingBox.origin.axis1
              || (*it)->boundingBox.origin.axis2 + (*it)->boundingBox.edgeLengths.axis2 
                  < (*it2)->boundingBox.origin.axis2
              || (*it2)->boundingBox.origin.axis2 + (*it2)->boundingBox.edgeLengths.axis2 
                  < (*it)->boundingBox.origin.axis2
              || (*it)->boundingBox.origin.axis3 + (*it)->boundingBox.edgeLengths.axis3 
                  < (*it2)->boundingBox.origin.axis3
              || (*it2)->boundingBox.origin.axis3 + (*it2)->boundingBox.edgeLengths.axis3 
                  < (*it)->boundingBox.origin.axis3))
            {
              //printf("Collision possible at Iteration: %d between objects %d and %d\n", 
              //    iterations, (*it)->id, (*it2)->id);
              if(testCollision(*(*it), *(*it2), num_dim, timestep)){
                //TODO

              }
            }


          break;
          case 2:
            if(!(
              (*it)->boundingBox.origin.axis1 + (*it)->boundingBox.edgeLengths.axis1 
                  < (*it2)->boundingBox.origin.axis1
              || (*it2)->boundingBox.origin.axis1 + (*it2)->boundingBox.edgeLengths.axis1 
                  < (*it)->boundingBox.origin.axis1
              || (*it)->boundingBox.origin.axis2 + (*it)->boundingBox.edgeLengths.axis2 
                  < (*it2)->boundingBox.origin.axis2
              || (*it2)->boundingBox.origin.axis2 + (*it2)->boundingBox.edgeLengths.axis2 
                  < (*it)->boundingBox.origin.axis2))
            {
              //printf("Collision possible at Iteration: %d between objects %d and %d\n", 
              //    iterations, (*it)->id, (*it2)->id);
              //if collision, mark second object to be removed and add mass and energy and calculate new trajectory
              if(testCollision(*(*it), *(*it2), num_dim, timestep)){
                (*it2)->toDelete = true;
                //conservation of momentum??
                coordinate lhsForce ((*it)->velocity.axis1 * (*it)->mass, (*it)->velocity.axis2 * (*it)->mass);
                coordinate rhsForce ((*it2)->velocity.axis1 * (*it2)->mass, (*it2)->velocity.axis2 * (*it2)->mass);
                lhsForce.axis1 += rhsForce.axis1;
                lhsForce.axis2 += rhsForce.axis2;
                //add mass
                (*it)->mass += (*it2)->mass;
                
                //set new velocity
                (*it)->velocity.axis1 = lhsForce.axis1 / (*it)->mass;
                (*it)->velocity.axis2 = lhsForce.axis2 / (*it)->mass;

                //combine area to find new radius
                (*it)->radius = sqrt( pow((*it)->radius,2) + pow((*it2)->radius,2) );
              }
            }
          break;
          }
        }
      }
    } else {
      for ( auto it = children.begin(); it < children.end(); ++it){
        (*it)->checkCollisions(iterations, num_dim, timestep);
      }
    }

  }


private:		
	bool leaf_node;
  coordinate division;
	std::vector<Octree<T>*> children;  

  void getMaxDepth(int current, int &max){
    if(current > max){
      max = current;
    }
    for ( auto it = children.begin(); it < children.end(); ++it){
      (*it)->getMaxDepth(current+1, max);
    }
  }
  void getMaxLeaves(int &max){
    if(leaf_node && leaves.size() > max){
      max = leaves.size();
    }
    for ( auto it = children.begin(); it < children.end(); ++it){
      (*it)->getMaxLeaves(max);
    }
  }
  void getTotalLeaves(int &total){
    total += leaves.size();
    for ( auto it = children.begin(); it < children.end(); ++it){
      (*it)->getTotalLeaves(total);
    }
  }
};


#endif
