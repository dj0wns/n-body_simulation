//Class containing relevant information for a body in the system
#include <cstdio>

#ifndef BODY_H
#define BODY_H
 
//consider struct of arrays rather than array of strucs
struct coordinate{
  double axis1;
  double axis2;
  double axis3;
  coordinate () : axis1(0.0), axis2(0.0), axis3(0.0){};
  //one axis
  coordinate(double axis1) : 
    axis1(axis1), axis2(0.0), axis3(0.0) {};
  //two axis
  coordinate(double axis1, double axis2) : 
    axis1(axis1), axis2(axis2), axis3(0.0){};
  //three axis
  coordinate(double axis1, double axis2, double axis3) :
    axis1(axis1), axis2(axis2), axis3(axis3){};
};

struct bounding_box{
  coordinate origin;
  coordinate edgeLengths;
  double area(){
    return edgeLengths.axis1 * edgeLengths.axis2;
  }
  double volume(){
    return edgeLengths.axis1 * edgeLengths.axis2 * edgeLengths.axis3;
  }
  void print(int num_dims, int id){
    switch(num_dims){
      case 2:
        printf ("Bounding Box for id: %d\n\torigin: %f\t%f\t\n\tedges: %f\t%f\t\n\tarea: %f\n\n", 
          id, origin.axis1, origin.axis2, edgeLengths.axis1, edgeLengths.axis2, area()); 
      break;
      case 3:
        printf ("Bounding Box for id: %d\n\torigin: %f\t%f\t%f\t\n\tedges: %f\t%f\t%f\t\n\tvolume: %f\n\n", 
          id, origin.axis1, origin.axis2, origin.axis3,
          edgeLengths.axis1, edgeLengths.axis2, edgeLengths.axis3,
          volume()); 
      break;
    }
  }
};

struct body{
  coordinate location;
  coordinate velocity;
  coordinate addedVelocity;
  bounding_box boundingBox;
  double mass;
  double radius;
  int id; //unique id for this object
  bool toDelete;
  //one axis
  body(double axis1, double vel1, double mass, double radius, int id) : 
    location(axis1), velocity(vel1), addedVelocity(0), mass(mass), radius(radius), id(id), toDelete(false){};
  //two axis
  body(double axis1, double axis2, 
      double vel1, double vel2, 
      double mass, double radius, int id) : 
    location(axis1,axis2), velocity(vel1,vel2), addedVelocity(0,0), mass(mass), radius(radius), id(id), toDelete(false){};
  //three axis
  body(double axis1, double axis2, double axis3,
      double vel1, double vel2, double vel3,  
      double mass, double radius, int id) : 
    location(axis1,axis2,axis3), velocity(vel1, vel2, vel3), addedVelocity(0,0,0), mass(mass), radius(radius), id(id), toDelete(false){};
  void toString(int num_dim, char* output);
  void print(int num_dim);
  void calculateBoundingBox(int num_dim, double time_step);
  void step(int num_dim, double time_step);

};


//wrap comparison functions in namespace to allow easy insertion of alternatives to body
namespace body_comp {
  bool comp1(body*,body*);
  bool comp2(body*,body*);
  bool comp3(body*,body*);
  bool compMiddleBound1(body*,body*);
  bool compMiddleBound2(body*,body*);
  bool compMiddleBound3(body*,body*);
}

#endif 
