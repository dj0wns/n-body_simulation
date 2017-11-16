#include <cstdio>

#include "Octree.h" 
#include "Calculations.h"
#include "File_IO.h"
#include "Body.h"

//Usage:
//./N-Body_Sim [infile] [outfile] [number of timesteps to simulate] [length of timestep in seconds] [accuracy 0-1]

//Notes:
//May change arguments to follow a more standard procedure


void execute(Octree<body> &octree, std::vector<body> &object_list, int iteration, int num_dim, int iterations, double time_step);

int main(int argc, char ** argv){
  if(argc < 5){
    printf("Insufficient Argument Number.\nUsage:\n./N-Body_Sim [infile] [outfile] [number of timesteps to simulate] [length of timestep in seconds] [accuracy 0-1]\n");
    return 0;
  }

  std::vector<body> object_list;

  int num_dim;
  int iterations = atoi(argv[3]);
  double time_step = atoi(argv[4]);
  
  read_infile(argv[1], object_list, num_dim);
  
  write_logfile(argv[2], object_list, 0, num_dim);
  for(int i = 0; i < iterations; i++){
    Octree<body> octree(&object_list);
    execute(octree, object_list, i, num_dim, iterations, time_step);
    write_logfile(argv[2], object_list, i+1, num_dim);
    //octree.print(i);
  } 
  
  write_outfile(argv[2], object_list, num_dim);

  return 0;
}

void execute(Octree<body> &octree, std::vector<body> &object_list, int iteration, int num_dim, int iterations, double time_step){
  //iterate through all unique pairs of items in the set
  for(auto it = object_list.begin(); it < object_list.end(); ++it){
    for(auto it2 = it+1; it2 < object_list.end(); ++it2){
      calculate_gravitational_velocity_change(*it, *it2, num_dim, time_step);
    }
  }
  //calculate bounding boxes
  for(auto it = object_list.begin(); it < object_list.end(); ++it){
    it->calculateBoundingBox(num_dim, time_step);
    //it->boundingBox.print(num_dim, it->id);
  }
  //build octree
  for(auto it = object_list.begin(); it < object_list.end(); ++it){
    octree.leaves.emplace_back(&(*it));
  }
  octree.create(num_dim);
  //check for collisions
  octree.checkCollisions(iteration,num_dim,time_step);
  for(auto it = object_list.begin(); it < object_list.end(); ++it){
    it->step(num_dim, time_step);
  }
  std::vector<body> new_object_list;
  for (auto it : object_list){
    if( !it.toDelete){
      new_object_list.emplace_back(it);
    } else{
      //maybe?
    }
  }
  object_list.clear();
  object_list = new_object_list;

}
