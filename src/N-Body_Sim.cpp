#include <cstdio>
#include <unistd.h>
#include <cstdint>

#include "Arguments.h"
#include "Octree.h" 
#include "Calculations.h"
#include "File_IO.h"
#include "Body.h"

//Usage:
//./N-Body_Sim [infile] [outfile] [number of timesteps to simulate] [length of timestep in seconds] [accuracy 0-1]

//Notes:
//May change arguments to follow a more standard procedure


bool parseArguments(int argc, char ** argv, Arguments &arguments);
void execute(Octree<body> &octree, std::vector<body> &object_list, int iteration, uint32_t num_dim, int iterations, double timeStep, bool collisionDetect);

int main(int argc, char ** argv){
  Arguments arguments;
  if (!parseArguments(argc, argv, arguments)){
    return 0;
  }
  
  std::vector<body> object_list;

  uint32_t num_dim;
  int iterations = arguments.iterations;
  double timeStep = arguments.timeStep;
  bool collisionDetect = arguments.collisionDetect;

  read_infile(arguments.inputPath, object_list, num_dim);
  
  write_logfile(arguments.outputPath, object_list, 0, num_dim);
  for(int i = 0; i < iterations; i++){
    Octree<body> octree(&object_list);
    execute(octree, object_list, i, num_dim, iterations, timeStep, collisionDetect);
    write_logfile(arguments.outputPath, object_list, i+1, num_dim);
    //octree.print(i);
  } 
  
  write_outfile(arguments.outputPath, object_list, num_dim);

  return 0;
}

void printUsageFormat(char * executableName){
  printf("Usage:\n" 
          "%s -i [Input File] -o [Output File] -t [time step] -n [number of iterations] [flags]\n"
          "\tValid Flags:\n"
          "\t\t-c : Enable Collision Detection\n"
          , executableName);
}

bool parseArguments(int argc, char ** argv, Arguments &arguments){
  int c;
  while (( c = getopt (argc, argv, "i:o:n:t:c")) != -1){
    switch (c){
      case 'i':
        arguments.inputPath = optarg;
        break;
      case 'o':
        arguments.outputPath = optarg;
        break;
      case 'n':
        arguments.iterations = atoi(optarg);
        break;
      case 't':
        arguments.timeStep = atoi(optarg);
        break;
      case 'c':
        arguments.collisionDetect = true;
        break;
      case '?':
        printUsageFormat(argv[0]);
        printf("\nInvalid Argument %c, %s", optopt, optarg);
        return false;
        break;
    }
  }
  
  //Validate Input
  if (arguments.inputPath == 0){
    printUsageFormat(argv[0]);
    return false;
  }
  if (arguments.outputPath == 0){
    printUsageFormat(argv[0]);
    return false;
  }
  if (arguments.iterations <= 0){
    printUsageFormat(argv[0]);
    return false;
  }
  if (arguments.timeStep <= 0){
    printUsageFormat(argv[0]);
    return false;
  }
  

  return true;
}

void execute(Octree<body> &octree, std::vector<body> &object_list, int iteration, uint32_t num_dim, int iterations, double timeStep, bool collisionDetect){
  //iterate through all unique pairs of items in the set
  for(auto it = object_list.begin(); it < object_list.end(); ++it){
    for(auto it2 = it+1; it2 < object_list.end(); ++it2){
      calculate_gravitational_velocity_change(*it, *it2, num_dim, timeStep);
    }
  }
  
  //Detect collisions and calculate mass and force changed
  if (collisionDetect){
    //calculate bounding boxes
    for(auto it = object_list.begin(); it < object_list.end(); ++it){
      it->calculateBoundingBox(num_dim, timeStep);
      //it->boundingBox.print(num_dim, it->id);
    }
    //build octree
    for(auto it = object_list.begin(); it < object_list.end(); ++it){
      octree.leaves.emplace_back(&(*it));
    }
    octree.create(num_dim);
    //check for collisions
    octree.checkCollisions(iteration,num_dim,timeStep);
  }

  //Step through all objects and move them according to their velocity
  for(auto it = object_list.begin(); it < object_list.end(); ++it){
    it->step(num_dim, timeStep);
  }

  //rebuild object list to remove collided objects
  if (collisionDetect){
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
}
