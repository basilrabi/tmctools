#ifndef UTILS_H
#define UTILS_H

#include <Rcpp.h>
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

// Executes a Postgres query using a connection parameter string
bool sendQuery ( const std::string& connectionParameters,
                 const std::string& query );

// Generate a random string at a certain length.
std::string randomString( const unsigned int& len );

// Read a string file and output a vector of points.
std::vector<DirVector> readSTR( const std::string& strFile );

// Read a dtm file and create a vector of triangles using a vector of points.
std::vector<Triangle> pointToTri ( std::vector<DirVector>& points,
                                   const std::string& dtmFile );

// Return the number of rows of a Postgres table
unsigned int nrow( const std::string& connectionParameters,
                   const std::string& schema,
                   const std::string& table );

// Write a ply header into a file
void writePlyHeader( const std::string& plyFile,
                     const unsigned int& npoints,
                     const unsigned int& nfaces );

// Append faces to a ply file using a PostGIS table of point set
void writePlyFace( const std::string& plyFile,
                   const std::string& connectionParameters,
                   const std::string& schemaPointSet,
                   const std::string& tablePointSet,
                   const std::string& schemaPoint,
                   const std::string& tablePoint );

// Append vertices to a ply file using a PostGIS table of points
void writePlyVertex( const std::string& plyFile,
                     const std::string& connectionParameters,
                     const std::string& schema,
                     const std::string& table );

#endif
