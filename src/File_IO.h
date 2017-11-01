//header containing all file input and output operations

#include <fstream>
#include <string>
#include <vector>

#include "Octree.h"
#include "Body.h"

#ifndef FILEIO_H
#define FILEIO_H

bool read_infile(char* path, std::vector<body> &object_list, int &num_dim);

bool write_outfile(char* path, std::vector<body> &object_list, int &num_dim);

bool write_logfile(char* path, std::vector<body> &object_list, int step, int &num_dim);



#endif
