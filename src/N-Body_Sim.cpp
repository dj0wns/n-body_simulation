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

//Calculation Error Notes:
//There seems to be minor floating point error when utilizing different numbers of mpi threads. no idea why - maybe my math is wrong but I cant figure out the culprit, seems to propogate slowly after second iteration. Maybe its skipping a multiply or something because its consistent across number of threads
// -input has been checked to be correct and correctly communicated across threads
// -performing more steps over a shorter period (10x more steps and 1/10th the timestep size seems to reduce error drastically)

//General Todos
//Add timer functionality to time how long each iteration took
//fix collision for multi threading
//consider adding dedicated i/o rank so that the worker threads arent all losing time to I/O
//unroll loops to remove unnecessary branching

bool parseArguments(int argc, char ** argv, Arguments &arguments);
void addLocalForces(body *object_list, const int length, const uint32_t num_dim, const double timeStep);
void addForces(body *to_list, body *from_list, const int toLen, const int fromLen, const uint32_t num_dim, const double timeStep);
void execute(Octree<body> &octree, body **object_list, int &length, const int iteration, const int num_dim, const int iterations, const double timeStep, const bool collisionDetect);

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

  //All threads can parse arguments - reduces communication - might want to have a single thread parse before initing mpi
  Arguments arguments;
  if (!parseArguments(argc, argv, arguments)){
    return 0;
  }
  
  //buffer to hold a processes' local set of bodies
  body * localBuf;
  //use alternating buffers to reduce number of memcpys
  body * buf0;
  body * buf1;

  uint32_t num_dim;
  int iterations = arguments.iterations;
  double timeStep = arguments.timeStep;
  bool collisionDetect = arguments.collisionDetect;
  int dataLength;
  int dataModulus;
  bool currentBuf = 0;

  //Rank 0 thread handles all I/O and prepares to distribute
  if (rank == 0) {
    read_infile(arguments.inputPath, &localBuf, &dataLength, num_dim);
    //TODO - have data length be dynamic to deal with shorter cases
    //TODO - add a difference between dataLength and maxDataLength
    //datalength = the total data entries divided by the number of tasks plus 1 if it has a remainder
    dataModulus = dataLength % numtasks;
    dataLength = (dataLength / numtasks) + !!(dataLength % numtasks);
    printf("Sending Data to %d ranks of max size %d\n", numtasks, dataLength);
    for (int i = 1; i < numtasks; i++){
      MPI_Send(&num_dim, 1, MPI_INT, i, 0, MPI_COMM_WORLD);
      MPI_Send(&dataLength, 1, MPI_INT, i, 1, MPI_COMM_WORLD);
      MPI_Send(&localBuf[i * dataLength], dataLength * sizeof(body), MPI_CHAR, i, 2, MPI_COMM_WORLD);
    }
    //allocate other buffer
    buf0 = (body*) malloc(dataLength * sizeof(body));
    buf1 = (body*) malloc(dataLength * sizeof(body));
  }

  //Non rank 0 tasks wait for data
  if (rank != 0) {
    //get num dims
    MPI_Recv(&num_dim, 1, MPI_INT, 0, 0, MPI_COMM_WORLD, &stat);
    //get length to receive
    MPI_Recv(&dataLength, 1, MPI_INT, 0, 1, MPI_COMM_WORLD, &stat);
    //malloc and receive that length
    localBuf = (body*) malloc(dataLength * sizeof(body));
    buf0 = (body*) malloc(dataLength * sizeof(body));
    buf1 = (body*) malloc(dataLength * sizeof(body));
    MPI_Recv(localBuf, dataLength * sizeof(body), MPI_CHAR, 0, 2, MPI_COMM_WORLD, &stat);
    printf ("Rank %d has received a data buffer of %d bodies: first ID = %d\n", rank, dataLength, localBuf[0].id);
  }
  
  //return answers to master thread to write out
  //use the currently unusued buf0 to store temp data
  if (rank == 0){
    write_logfile(arguments.outputPath, localBuf, dataLength, 0, num_dim);
    for (int j = 1; j < numtasks; j++){
      int tempDataLength;
      MPI_Recv(&tempDataLength, 1, MPI_INT, j, 7, MPI_COMM_WORLD, &stat);
      MPI_Recv(buf0, dataLength * sizeof(body), MPI_CHAR, j, 8, MPI_COMM_WORLD, &stat);
      append_logfile(arguments.outputPath, buf0, tempDataLength, 0, num_dim);
    }
  } else {
      MPI_Send(&dataLength, 1, MPI_INT, 0, 7, MPI_COMM_WORLD);
      MPI_Send(localBuf, dataLength * sizeof(body), MPI_CHAR, 0, 8, MPI_COMM_WORLD);
  }

  for(int i = 0; i < iterations; i++){
    //reset buffers
    currentBuf = false;
    addLocalForces(localBuf, dataLength, num_dim, timeStep);
    //pass local data to next thread and receive data on buf0
    if (rank == 0 && numtasks > 1){
      MPI_Send(&dataLength, 1, MPI_INT, (rank+1) % numtasks, 3, MPI_COMM_WORLD);
      MPI_Send(localBuf, dataLength * sizeof(body), MPI_CHAR, (rank+1) % numtasks, 4, MPI_COMM_WORLD);
      
      MPI_Recv(&dataLength, 1, MPI_INT, numtasks-1, 3, MPI_COMM_WORLD, &stat);
      MPI_Recv(buf0, dataLength * sizeof(body), MPI_CHAR, numtasks-1, 4, MPI_COMM_WORLD, &stat);
    } else if (numtasks > 1){
      MPI_Recv(&dataLength, 1, MPI_INT, rank-1, 3, MPI_COMM_WORLD, &stat);
      MPI_Recv(buf0, dataLength * sizeof(body), MPI_CHAR, rank-1, 4, MPI_COMM_WORLD, &stat);
      
      MPI_Send(&dataLength, 1, MPI_INT, (rank+1) % numtasks, 3, MPI_COMM_WORLD);
      MPI_Send(localBuf, dataLength * sizeof(body), MPI_CHAR, (rank+1)%numtasks, 4, MPI_COMM_WORLD);
    }
    //Do one iteration for each rank
    for (int j = 1; j < numtasks; j++){
      //move object list to next rank (increment and wrap)
      //TODO optimize mpi comms with non blocking sends
      
      addForces(localBuf, (currentBuf ? buf1 : buf0), dataLength, dataLength, num_dim, timeStep);

      if (rank == 0){
        MPI_Send(&dataLength, 1, MPI_INT, (rank+1) % numtasks, 3, MPI_COMM_WORLD);
        MPI_Send((currentBuf ? buf1 : buf0), dataLength * sizeof(body), MPI_CHAR, (rank+1) % numtasks, 4, MPI_COMM_WORLD);
        
        MPI_Recv(&dataLength, 1, MPI_INT, numtasks-1, 3, MPI_COMM_WORLD, &stat);
        //receive in opposite buf
        MPI_Recv((currentBuf ? buf0 : buf1), dataLength * sizeof(body), MPI_CHAR, numtasks-1, 4, MPI_COMM_WORLD, &stat);
      }else{
        MPI_Recv(&dataLength, 1, MPI_INT, rank-1, 3, MPI_COMM_WORLD, &stat);
        MPI_Recv((currentBuf ? buf0 : buf1), dataLength * sizeof(body), MPI_CHAR, rank-1, 4, MPI_COMM_WORLD, &stat);
        
        MPI_Send(&dataLength, 1, MPI_INT, (rank+1) % numtasks, 3, MPI_COMM_WORLD);
        MPI_Send((currentBuf ? buf1 : buf0), dataLength * sizeof(body), MPI_CHAR, (rank+1)%numtasks, 4, MPI_COMM_WORLD);
      }
      //switch buffers
      currentBuf = !currentBuf;
    }
    
    Octree<body> octree(localBuf);
    execute(octree, &localBuf, dataLength, i, num_dim, iterations, timeStep, collisionDetect);
    //Collision detect
    
    //return answers to master thread to write out
    if (i % arguments.outputIterations == 0 && rank == 0){
      printf("Iteration %d Completed!\n", i);
      write_logfile(arguments.outputPath, localBuf, dataLength, ((i/arguments.outputIterations)+1), num_dim);
      for (int j = 1; j < numtasks; j++){
        MPI_Recv(&dataLength, 1, MPI_INT, j, 7, MPI_COMM_WORLD, &stat);
        MPI_Recv(buf0, dataLength * sizeof(body), MPI_CHAR, j, 8, MPI_COMM_WORLD, &stat);
        append_logfile(arguments.outputPath, buf0, dataLength, ((i/arguments.outputIterations)+1), num_dim);
      }
    } else if (i % arguments.outputIterations == 0){
        MPI_Send(&dataLength, 1, MPI_INT, 0, 7, MPI_COMM_WORLD);
        MPI_Send(localBuf, dataLength * sizeof(body), MPI_CHAR, 0, 8, MPI_COMM_WORLD);
    }
    
  
  } 
  
  //TODO Rank 0 write out resulting file
  if (rank == 0) {
    write_outfile(arguments.outputPath, localBuf, dataLength, num_dim);
  }
  free(localBuf);
  free(buf0);
  free(buf1);


  MPI_Finalize();
  

  return 0;
}

void printUsageFormat(char * executableName){
  printf("Usage:\n" 
          "%s -i [Input File] -o [Output File] -t [time step] -n [number of iterations] -O [number of iterations per filewrite] [flags]\n"
          "\tValid Flags:\n"
          "\t\t-c : Enable Collision Detection\n"
          , executableName);
}

bool parseArguments(int argc, char ** argv, Arguments &arguments){
  int c;
  while (( c = getopt (argc, argv, "i:o:O:n:t:c")) != -1){
    switch (c){
      case 'i':
        arguments.inputPath = optarg;
        break;
      case 'o':
        arguments.outputPath = optarg;
        break;
      case 'O':
        arguments.outputIterations = atoi(optarg);
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

void addLocalForces(body *object_list, const int length, const uint32_t num_dim, const double timeStep){
  for(int i = 0; i < length; ++i){
    for(int j = i+1; j < length; ++j){
      calculate_gravitational_velocity_change(object_list[i], object_list[j], num_dim, timeStep);
    }
  }
}

void addForces(body *to_list, body *from_list, const int toLen, const int fromLen, const uint32_t num_dim, const double timeStep){
  for(int i = 0; i < toLen; ++i){
    for(int j = 0; j < fromLen; ++j){
      calculate_gravitational_velocity_change(to_list[i], from_list[j], num_dim, timeStep);
    }
  }
}

void execute(Octree<body> &octree, body **object_list, int &length, const int iteration, const int num_dim, const int iterations, const double timeStep, const bool collisionDetect){
  //TODO fix collision detection for threaded workloads
  //Detect collisions and calculate mass and force changed
  if (collisionDetect){
    //calculate bounding boxes
    for(int i = 0; i < length; ++i){
      (*object_list)[i].calculateBoundingBox(num_dim, timeStep);
    }
    //build octree
    for(int i = 0; i < length; ++i){
      octree.leaves.emplace_back(&((*object_list)[i]));
    }
    octree.create(num_dim);
    //check for collisions
    octree.checkCollisions(iteration,num_dim,timeStep);
  }

  //Step through all objects and move them according to their velocity
  for(int i = 0; i < length; ++i){
    (*object_list)[i].step(num_dim, timeStep);
  }

  //rebuild object list to remove collided objects
  //TODO remove vector logic
  if (collisionDetect){
    int index = 0;
    body * tempBuffer = (body*) malloc(length * sizeof(body));
    for (int i = 0; i < length; ++i){
      if( !(*object_list)[i].toDelete){
        tempBuffer[index] = (*object_list)[i];
        index++;
      } else{
        //maybe?
      }
    }
    //should be faster to build a new array and replace than to delete in place
    free(object_list);
    (*object_list) = tempBuffer;
  }
}
