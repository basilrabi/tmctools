#ifndef UTILS_H
#define UTILS_H

#include <Rcpp.h>
#include <fstream>
#include <string>
#include <vector>
#include "dirVector.h"
#include "triangle.h"

// Convert triangles to dataframe with columns:
//  1. polygon     : polygon in ewkt or wkt format
//  2. edge_length : length of the longest edge
//  3. slope_angle : angle in radians between z-axis and the polygon normal
Rcpp::DataFrame triangleToDataFrame( std::vector<Triangle>& triangles,
                                     const std::string& srid );

// Does the file exist?
bool fileExists( const std::string& name );

// Read a string file and output a vector of points.
std::vector<DirVector> readSTR( const std::string& strFile );

// Read a dtm file and create a vector of triangles using a vector of points.
std::vector<Triangle> pointToTri ( std::vector<DirVector>& points,
                                   const std::string& dtmFile );

#endif
