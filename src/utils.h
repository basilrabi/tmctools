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
struct int5 { int8_t x, y, z; };
struct int6 { uint8_t x, y, z; };

// Convert triangles to dataframe with columns:
//  1. area_2d     : 2D area of the triangle
//  2. edge_length : length of the longest edge
//  3. slope_angle : angle in radians between z-axis and the polygon normal
//  4. polygon     : polygon in ewkt or wkt format
template <class T>
Rcpp::DataFrame triangleToDataFrame( std::vector<T>& triangles,
                                     const std::string& srid )
{
  Rcpp::NumericVector area2d ( triangles.size() );
  Rcpp::NumericVector edge_length ( triangles.size() );
  Rcpp::NumericVector slope_angle ( triangles.size() );
  Rcpp::StringVector polygon ( triangles.size() );

  for ( unsigned int i = 0; i < triangles.size(); i++ )
  {
    area2d[i] = triangles[i].area2d();
    slope_angle[i] = triangles[i].slopeAngle();
    edge_length[i] = triangles[i].longestEdge();
    slope_angle[i] = triangles[i].slopeAngle();
    if ( srid != "" )
      polygon[i] = triangles[i].ewkt( srid );
    else
      polygon[i] = triangles[i].wkt();
  }

  Rcpp::DataFrame out = Rcpp::DataFrame::create(
    Rcpp::Named( "area_2d" ) = area2d,
    Rcpp::Named( "edge_length" ) = edge_length,
    Rcpp::Named( "slope_angle" ) = slope_angle,
    Rcpp::Named( "polygon" ) = polygon,
    Rcpp::Named( "stringsAsFactors" ) = false
  );

  return out;
}

// Does the file exist?
bool fileExists( const std::string& name );

// Executes a Postgres query using a connection parameter string
bool sendQuery ( const std::string& connection_parameters,
                 const std::string& query );

// Computes the slope of the triangle plane with respect to the z-axis given
// two sides of a triangle.
double slopeAngleBase( const DirVector& a, const DirVector& b);

// Generate a random string at a certain length.
std::string randomString( const unsigned int& len );

// Read a string file and output a vector of points.
std::vector<DirVector> readSTR( const std::string& str_file );

// Read a dtm file and create a vector of triangles using a vector of points.
std::vector<Triangle> pointToTri ( std::vector<DirVector>& points,
                                   const std::string& dtm );

// Return the number of rows of a Postgres table
unsigned int nrow( const std::string& connection_parameters,
                   const std::string& schema,
                   const std::string& table );

// Read the PLY file and store the vertices and faces in vectors.
void readPlyFile( const std::string& in_ply,
                  std::vector<DirVector>& out_vertices,
                  std::vector<TriangleIndex>& out_faces,
                  bool assign_triangle );

// Write a binary ply
void writePlyBinFromDB( const std::string& prefix,
                        const std::string& connection_parameters);

// Write a ply header into a file
void writePlyHeader( std::ofstream& ply,
                     const unsigned int& n_points,
                     const unsigned int& n_faces );

void writePlyHeaderFromDB( const std::string& ply,
                           const unsigned int& n_points,
                           const unsigned int& n_faces );

// Append faces to a ply file using a PostGIS table of point set
void writePlyFaceFromDB( const std::string& connection_parameters,
                         const std::string& prefix );

// Append vertices to a ply file using a PostGIS table of points
void writePlyVertexFromDB( const std::string& connection_parameters,
                           const std::string& prefix,
                           const unsigned int& digits );

#endif
