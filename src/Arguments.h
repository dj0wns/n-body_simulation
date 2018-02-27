
//Class to contain arguments parsed in ArgumentParser

class Arguments {
  
  public:

  Arguments() : inputPath(0), outputPath(0), outputIterations(1), collisionDetect(false), iterations(-1), timeStep(-1) {};

  const char * inputPath;
  const char * outputPath;
  
  int outputIterations;
  
  bool collisionDetect;
  
  double iterations;
  double timeStep;
};
