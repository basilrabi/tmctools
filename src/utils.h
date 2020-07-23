#ifndef UTILS_H
#define UTILS_H

#include <fstream>
#include <string>
#include <vector>
#include "dirVector.h"
#include "triangle.h"

// Does the file exist?
bool fileExists( const std::string& name );

// Read a string file and output a vector of points.
std::vector<DirVector> readSTR( const std::string& strFile );

// Read a dtm file and create a vector of triangles using a vector of points.
std::vector<Triangle> pointToTri ( std::vector<DirVector>& points,
                                   const std::string& dtmFile );

#endif
