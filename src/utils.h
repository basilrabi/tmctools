#ifndef UTILS_H
#define UTILS_H

#include <fstream>
#include <string>
#include <vector>
#include "dirVector.h"

// Does the file exist?
bool fileExists( const std::string& name );

// Read a string file and output a vector of points.
std::vector<DirVector> readSTR( std::string strFile );

#endif
