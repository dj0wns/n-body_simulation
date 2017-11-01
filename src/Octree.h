//header containing Octree structure and related operations

#ifndef OCTREE_H
#define OCTREE_H

#include <vector>
#include <cstdio>
#include "Body.h"

#include <algorithm>


template <typename T> class Octree  {
public:
	//use emplace to add objects to list - avoid copy 
	std::vector<T> *object_list; //contains list of all objects - shared among all instances of Octree
	std::vector<T*> leaves; //should be empty unless a leaf node


	//default constructor
	Octree() : leaf_node(false) {};
	
  //default constructor
	Octree( std::vector<T> *object_list) : object_list(object_list), leaf_node(false){};

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
  void create(int num_dim){
    //calculate average  of bounding box middle for split - should be good enough
    int count = 0;
    //if too few objects not worth splitting
    if(leaves.size() <= 2<<(num_dim-1)){
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
                + (axis2>division.axis3)*4]->leaves;
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

    //leaves.clear();
    for ( auto it = children.begin(); it < children.end(); ++it){
      (*it)->create(num_dim);
    }
    
  }
  
  void checkCollisions(int iterations, int num_dim){
    if(leaf_node){
      for ( auto it = leaves.begin(); it < leaves.end(); ++it){
        for ( auto it2 = it+1; it2 < leaves.end(); ++it2){
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
              printf("Collision possible at Iteration: %d between objects %d and %d\n", 
                  iterations, (*it)->id, (*it2)->id);
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
              printf("Collision possible at Iteration: %d between objects %d and %d\n", 
                  iterations, (*it)->id, (*it2)->id);
            }
          break;
          }
        }
      }
    } else {
      for ( auto it = children.begin(); it < children.end(); ++it){
        (*it)->checkCollisions(iterations, num_dim);
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
