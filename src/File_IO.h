//header containing all file input and output operations

#include <fstream>
#include <string>
#include <vector>

#include "Octree.h"
#include "Body.h"

#ifndef FILEIO_H
#define FILEIO_H

bool read_infile(const char* path, body **object_list, int *length, uint32_t &num_dim);

bool write_outfile(const char* path, body *object_list, int length,  uint32_t &num_dim);

bool write_logfile(const char* path, body *object_list, int length, int step, uint32_t &num_dim);
bool append_logfile(const char* path, body *object_list, int length, int step, uint32_t &num_dim);



#endif
