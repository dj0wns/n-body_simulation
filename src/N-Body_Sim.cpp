#include <cstdio>
#include <unistd.h>
#include <cstdint>

#include "mpi.h"
#include "Arguments.h"
#include "Octree.h" 
#include "Calculations.h"
#include "File_IO.h"
#include "Body.h"

//Usage:
//./N-Body_Sim [infile] [outfile] [number of timesteps to simulate] [length of timestep in seconds] [accuracy 0-1]

//Notes:
//There seems to be minor floating point error when utilizing mpi threads. no idea why - maybe my math is wrong but I cant figure out the culprit

//General Todos
//Add timer functionality to time how long each iteration took
//fix collision for multi threading

bool parseArguments(int argc, char ** argv, Arguments &arguments);
void addLocalForces(std::vector<body> &object_list, const uint32_t num_dim, const double timeStep);
void addForces(std::vector<body> &from_list, std::vector<body> &to_list, const uint32_t num_dim, const double timeStep);
void execute(Octree<body> &octree, std::vector<body> &object_list, int iteration, uint32_t num_dim, int iterations, double timeStep, bool collisionDetect);

int main(int argc, char ** argv){
  int numtasks;
  int rank;
  int len;
  int rc;

  char hostname[MPI_MAX_PROCESSOR_NAME];
 
  MPI_Status stat;

  MPI_Init(&argc, &argv);

  MPI_Comm_size(MPI_COMM_WORLD, &numtasks);

  MPI_Comm_rank(MPI_COMM_WORLD, &rank);

  MPI_Get_processor_name(hostname, &len);

  //All threads can parse arguments - reduces communication I guess
  Arguments arguments;
  if (!parseArguments(argc, argv, arguments)){
    return 0;
  }
  //TODO replace resize with reserve??
  std::vector<body> object_list;
  std::vector<body> local_object_list;
  std::vector<body> temp_object_list;
  

  uint32_t num_dim;
  int iterations = arguments.iterations;
  double timeStep = arguments.timeStep;
  bool collisionDetect = arguments.collisionDetect;
  int dataLength;
  int dataModulus;
 

  //Rank 0 thread handles all I/O and prepares to distribute
  if (rank == 0) {
    read_infile(arguments.inputPath, object_list, num_dim);
    //TODO - have data length be dynamic to deal with shorter cases
    //datalength = the total data entries divided by the number of tasks plus 1 if it has a remainder
    dataModulus = object_list.size() % numtasks;
    dataLength = (object_list.size() / numtasks) + !!(object_list.size() % numtasks);
    printf("Sending Data to %d ranks of max size %d\n", numtasks, dataLength);
    for (int i = 1; i < numtasks; i++){
      MPI_Send(&num_dim, 1, MPI_INT, i, 0, MPI_COMM_WORLD);
      MPI_Send(&dataLength, 1, MPI_INT, i, 1, MPI_COMM_WORLD);
      MPI_Send(&object_list[i * dataLength], dataLength * sizeof(body), MPI_CHAR, i, 2, MPI_COMM_WORLD);
    }
    object_list.resize(dataLength);
  }

  //Non rank 0 tasks wait for data
  if (rank != 0) {
    //get num dims
    MPI_Recv(&num_dim, 1, MPI_INT, 0, 0, MPI_COMM_WORLD, &stat);
    //get length to receive
    MPI_Recv(&dataLength, 1, MPI_INT, 0, 1, MPI_COMM_WORLD, &stat);
    //malloc and receive that length
    object_list.resize(dataLength);
    MPI_Recv(&object_list[0], dataLength * sizeof(body), MPI_CHAR, 0, 2, MPI_COMM_WORLD, &stat);
    printf ("Rank %d has received a data buffer of %d bodies: first ID = %d\n", rank, dataLength, object_list[0].id);
  }
  
  //return answers to master thread to write out
  if (rank == 0){
    write_logfile(arguments.outputPath, object_list, 0, num_dim);
    for (int j = 1; j < numtasks; j++){
      MPI_Recv(&dataLength, 1, MPI_INT, j, 7, MPI_COMM_WORLD, &stat);
      temp_object_list.resize(dataLength);
      MPI_Recv(&temp_object_list[0], dataLength * sizeof(body), MPI_CHAR, j, 8, MPI_COMM_WORLD, &stat);
      append_logfile(arguments.outputPath, temp_object_list, 0, num_dim);
    }
  } else {
      MPI_Send(&dataLength, 1, MPI_INT, 0, 7, MPI_COMM_WORLD);
      MPI_Send(&object_list[0], dataLength * sizeof(body), MPI_CHAR, 0, 8, MPI_COMM_WORLD);
  }

  for(int i = 0; i < iterations; i++){
    local_object_list = object_list;
    addLocalForces(object_list, num_dim, timeStep);
    //Do one iteration for each rank
    for (int j = 1; j < numtasks; j++){
      //move object list to next rank (increment and wrap)
      //TODO optimize mpi comms with non blocking sends

      if (rank == 0){
        MPI_Send(&dataLength, 1, MPI_INT, (rank+1) % numtasks, 3, MPI_COMM_WORLD);
        MPI_Send(&object_list[0], dataLength * sizeof(body), MPI_CHAR, (rank+1) % numtasks, 4, MPI_COMM_WORLD);
        
        MPI_Recv(&dataLength, 1, MPI_INT, numtasks-1, 3, MPI_COMM_WORLD, &stat);
        object_list.resize(dataLength);
        MPI_Recv(&object_list[0], dataLength * sizeof(body), MPI_CHAR, numtasks-1, 4, MPI_COMM_WORLD, &stat);
      }else{
        temp_object_list = object_list;
        MPI_Recv(&dataLength, 1, MPI_INT, rank-1, 3, MPI_COMM_WORLD, &stat);
        object_list.resize(dataLength);
        MPI_Recv(&object_list[0], dataLength * sizeof(body), MPI_CHAR, rank-1, 4, MPI_COMM_WORLD, &stat);
        
        MPI_Send(&dataLength, 1, MPI_INT, (rank+1) % numtasks, 3, MPI_COMM_WORLD);
        MPI_Send(&temp_object_list[0], dataLength * sizeof(body), MPI_CHAR, (rank+1)%numtasks, 4, MPI_COMM_WORLD);
        
      }

      addForces(local_object_list, object_list, num_dim, timeStep);
    }
    //return to proper order for logging purposes
    if (numtasks > 1 && rank == 0){
      MPI_Send(&dataLength, 1, MPI_INT, (rank+1) % numtasks, 5, MPI_COMM_WORLD);
      MPI_Send(&object_list[0], dataLength * sizeof(body), MPI_CHAR, (rank+1) % numtasks, 6, MPI_COMM_WORLD);
      
      MPI_Recv(&dataLength, 1, MPI_INT, numtasks-1, 5, MPI_COMM_WORLD, &stat);
      object_list.resize(dataLength) ;
      MPI_Recv(&object_list[0], dataLength * sizeof(body), MPI_CHAR, numtasks-1, 6, MPI_COMM_WORLD, &stat);
    }else if (numtasks > 1){
      temp_object_list = object_list;
      MPI_Recv(&dataLength, 1, MPI_INT, rank-1, 5, MPI_COMM_WORLD, &stat);
      object_list.resize(dataLength);
      MPI_Recv(&object_list[0], dataLength * sizeof(body), MPI_CHAR, rank-1, 6, MPI_COMM_WORLD, &stat);
      
      MPI_Send(&dataLength, 1, MPI_INT, (rank+1) % numtasks, 5, MPI_COMM_WORLD);
      MPI_Send(&temp_object_list[0], dataLength * sizeof(body), MPI_CHAR, (rank+1)%numtasks, 6, MPI_COMM_WORLD);     
    }


    Octree<body> octree(&object_list);
    execute(octree, object_list, i, num_dim, iterations, timeStep, collisionDetect);
    //Collision detect
    
    //return answers to master thread to write out
    if (rank == 0){
      printf("Iteration %d Completed!\n", i);
      write_logfile(arguments.outputPath, object_list, i+1, num_dim);
      for (int j = 1; j < numtasks; j++){
        MPI_Recv(&dataLength, 1, MPI_INT, j, 7, MPI_COMM_WORLD, &stat);
        temp_object_list.resize(dataLength);
        MPI_Recv(&temp_object_list[0], dataLength * sizeof(body), MPI_CHAR, j, 8, MPI_COMM_WORLD, &stat);
        append_logfile(arguments.outputPath, temp_object_list, i+1, num_dim);
      }
    } else {
        MPI_Send(&dataLength, 1, MPI_INT, 0, 7, MPI_COMM_WORLD);
        MPI_Send(&object_list[0], dataLength * sizeof(body), MPI_CHAR, 0, 8, MPI_COMM_WORLD);
    }
    
  

  } 
  
  //TODO Rank 0 write out resulting file
  if (rank == 0) {
    write_outfile(arguments.outputPath, object_list, num_dim);
  }
  
  MPI_Finalize();
  
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

void addLocalForces(std::vector<body> &object_list, const uint32_t num_dim, const double timeStep){
  for(auto it = object_list.begin(); it < object_list.end(); ++it){
    for(auto it2 = it + 1; it2 < object_list.end(); ++it2){
      calculate_gravitational_velocity_change(*it, *it2, num_dim, timeStep);
    }
  }
}

void addForces(std::vector<body> &from_list, std::vector<body> &to_list, const uint32_t num_dim, const double timeStep){
  for(auto it = from_list.begin(); it < from_list.end(); ++it){
    for(auto it2 = to_list.begin(); it2 < to_list.end(); ++it2){
      calculate_gravitational_velocity_change(*it, *it2, num_dim, timeStep);
    }
  }
}

void execute(Octree<body> &octree, std::vector<body> &object_list, int iteration, uint32_t num_dim, int iterations, double timeStep, bool collisionDetect){
  //TODO fix collision detection for threaded workloads
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
