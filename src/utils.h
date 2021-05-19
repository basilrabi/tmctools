#ifndef UTILS_H
#define UTILS_H

#include <Rcpp.h>
#include <fstream>
#include <string>
#include <vector>
#include "dirVector.h"
#include "triangle.h"

struct float1 { float x, y, z; };
struct float2 { double x, y, z; };
struct int1 { int16_t x, y, z; };
struct int2 { uint16_t x, y, z; };
struct int3 { int32_t x, y, z; };
struct int4 { uint32_t x, y, z; };

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

// Computes the slope of the triangle plane with respect to the z-axis given
// two sides of a triangle.
double slopeAngleBase( const DirVector& a, const DirVector& b);

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

// Read the binary PLY file and store the vertices and faces in vectors.
void readPly( const std::string& inPly,
              std::vector<DirVector>& outVertices,
              std::vector<TriangleIndex>& outFaces );

// Write a ply header into a file
void writePlyHeader( std::ofstream& plyFile,
                     const unsigned int& npoints,
                     const unsigned int& nfaces );

void writePlyHeaderFromDB( const std::string& plyFile,
                           const unsigned int& npoints,
                           const unsigned int& nfaces );

// Append faces to a ply file using a PostGIS table of point set
void writePlyFaceFromDB( const std::string& plyFile,
                         const std::string& connectionParameters,
                         const std::string& schemaPointSet,
                         const std::string& tablePointSet,
                         const std::string& schemaPoint,
                         const std::string& tablePoint );

// Append vertices to a ply file using a PostGIS table of points
void writePlyVertexFromDB( const std::string& plyFile,
                           const std::string& connectionParameters,
                           const std::string& schema,
                           const std::string& table );

#endif
