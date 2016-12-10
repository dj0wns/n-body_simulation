//header containing Octree structure and related operations

#ifndef OCTREE_H
#define OCTREE_H

#include <vector>
#include <cstdio>

enum axis {X_Axis, Y_Axis, Z_Axis, NONE};


template <typename T> class Octree  {
public:
	//use emplace to add objects to list - avoid copy 
	std::vector<T> object_list; //should be empty unless a leaf node
	std::vector<bool(*)(T*,T*)> comparison_functions; //if multiple, uses quality of split to determine best


	//default constructor
	Octree() : leaf_node(false), left(0), right(0), division(NONE), mass(-1) {};

	void create(); //recursively create tree using objects in object_list
	
	void print(){//print contents of the tree along with other information
		printf("Size: %d\n", object_list.size());
		for(auto it = object_list.begin(); it < object_list.end(); ++it){
			it->print();

		}
	}
private:		
	bool leaf_node;

	Octree *left; //left children
	Octree *right; //right children
	
	axis division; //which axis the tree is divided on
	
	//center of mass of the node
	//coordinate center;
	double mass;
};


#endif

