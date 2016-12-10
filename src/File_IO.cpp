#include "File_IO.h"

bool write_outfile(char* path, Octree<body> &structure, int &num_dim){
	std::ofstream fout;
	fout.open(path);
	char output[256];

	fout << num_dim << "\t" << structure.object_list.size() << "\n";

	for(auto it = structure.object_list.begin(); it < structure.object_list.end(); ++it){
		it->toString(num_dim, output);	
		fout << output;

	}

}

bool read_infile(char* path, Octree<body> &structure, int &num_dim){
	std::ifstream fin;
	int num_entries;
	double axis1, axis2, axis3, mass;
	double vel1, vel2, vel3;
	int id;
		   
	fin.open(path);
	
	if(fin.good()){
		fin >> num_dim;
		fin >> num_entries;
	} else {
		return false;
	}
	structure.object_list.clear();
	structure.object_list.reserve(num_entries);

	for(int i = 0; i < num_entries && fin.good(); i++){
		switch(num_dim){
			case 1:
				fin >> axis1;
				fin >> vel1;
				fin >> mass;
				fin >> id;
				structure.object_list.emplace_back(body(axis1, vel1,  mass, id));
				break;
			case 2:
				fin >> axis1;
				fin >> axis2;
				fin >> vel1;
				fin >> vel2;
				fin >> mass;
				fin >> id;
				structure.object_list.emplace_back(body(axis1, axis2, vel1, vel2, mass, id));
				break;
			case 3:
				fin >> axis1;
				fin >> axis2;
				fin >> axis3;
				fin >> vel1;
				fin >> vel2;
				fin >> vel3;
				fin >> mass;
				fin >> id;
				structure.object_list.emplace_back(body(axis1, axis2, axis3, vel1, vel2, vel3, mass, id));
				break;
			default:
				return false;
		}
	}
	return true;
}

