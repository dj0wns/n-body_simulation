//Class containing relevant information for a body in the system

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

struct body{
	coordinate location;
	coordinate velocity;
	double mass;
	int id; //unique id for this object
	//one axis
	body(double axis1, double vel1, double mass, int id) : 
		location(axis1), velocity(vel1), mass(mass), id(id){};
	//two axis
	body(double axis1, double axis2, 
			double vel1, double vel2, 
			double mass, int id) : 
		location(axis1,axis2), velocity(vel1,vel2), mass(mass), id(id){};
	//three axis
	body(double axis1, double axis2, double axis3,
			double vel1, double vel2, double vel3, 	
			double mass, int id) : 
		location(axis1,axis2,axis3), velocity(vel1, vel2, vel3),  mass(mass), id(id){};
	void toString(int num_dim, char* output);
	void print(int num_dim);
	void step(int num_dim, double time_step);

};


//wrap comparison functions in namespace to allow easy insertion of alternatives to body
namespace body_comp {
	bool comp1(body*,body*);
	bool comp2(body*,body*);
	bool comp3(body*,body*);
}

#endif 
