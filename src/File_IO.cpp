#include "File_IO.h"
#include <string>
#include <cstring>
#include <algorithm>
#include <cstdlib>
#include <vector>

bool write_logfile(char* path, std::vector<body> &object_list, int step, int &num_dim){
	std::ofstream fout;
	std::string temp = path;
	char output[256];
	sprintf(output,"%s_%04d.dat",path,step);
	fout.open(output);
	for(auto it = object_list.begin(); it < object_list.end(); ++it){
		it->toString(num_dim, output);	
		fout << output;

	}
	fout.close();
	return true;
}



bool write_outfile(char* path, std::vector<body> &object_list, int &num_dim){
	std::ofstream fout;
	fout.open(path);
	char output[256];

	fout << num_dim << "\t" << object_list.size() << "\n";

	for(auto it = object_list.begin(); it < object_list.end(); ++it){
		it->toString(num_dim, output);	
		fout << output;

	}
	fout.close();
	return true;
}

bool read_infile(char* path, std::vector<body> &object_list, int &num_dim){
	std::ifstream fin;
	int num_entries;
	double axis1, axis2, axis3, mass;
	double vel1, vel2, vel3;
	double radius;
  int id;
		   
	fin.open(path);
	
	if(fin.good()){
		fin >> num_dim;
		fin >> num_entries;
	} else {
		return false;
	}
	object_list.clear();
	object_list.reserve(num_entries);

	for(int i = 0; i < num_entries && fin.good(); i++){
		switch(num_dim){
			case 1:
				fin >> axis1;
				fin >> vel1;
				fin >> mass;
        fin >> radius;
				fin >> id;
				object_list.emplace_back(body(axis1, vel1,  mass, radius, id));
				break;
			case 2:
				fin >> axis1;
				fin >> axis2;
				fin >> vel1;
				fin >> vel2;
				fin >> mass;
        fin >> radius;
				fin >> id;
				object_list.emplace_back(body(axis1, axis2, vel1, vel2, mass, radius, id));
				break;
			case 3:
				fin >> axis1;
				fin >> axis2;
				fin >> axis3;
				fin >> vel1;
				fin >> vel2;
				fin >> vel3;
				fin >> mass;
        fin >> radius;
				fin >> id;
				object_list.emplace_back(body(axis1, axis2, axis3, vel1, vel2, vel3, mass, radius, id));
				break;
			default:
				return false;
		}
	}
	fin.close();
	return true;
}

