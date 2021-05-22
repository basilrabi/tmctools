#ifndef TRIANGLE_H
#define TRIANGLE_H

#include <vector>
#include "dirVector.h"

// 3-dimensional triangle
class Triangle
{
public:

  // Vertices of the triangle
  DirVector *a, *b, *c;

  // Constructor
  Triangle();
  Triangle( DirVector&, DirVector&, DirVector& );

  // Desctructor
  ~Triangle(){}

  // Copy Constructor
  Triangle& operator= ( const Triangle& tri );

  // area in 2d
  double area2d() const;

  // length of edge opposite to vertex a
  double lengthA() const;

  // length of edge opposite to vertex b
  double lengthB() const;

  // length of edge opposite to vertex c
  double lengthC() const;

  // The length of the longest edge
  double longestEdge() const;

  // Angle between the z-axis and the polygon normal in radians
  double slopeAngle() const;

  // Output triangle in text
  std::string asText() const;

  // Output triangle in extended well-known text
  std::string ewkt( const std::string& srid ) const;

  // Output triangle in well-known text
  std::string wkt() const;
};


// triangle in a ply file
class TriangleIndex: public Triangle
{
public:

  // Vertices of the triangle
  uint32_t ai, bi, ci;

  // Constructor
  TriangleIndex();
  TriangleIndex( const uint32_t&, const uint32_t&, const uint32_t& );

  // Desctructor
  ~TriangleIndex(){}

  // Copy Constructor
  TriangleIndex& operator= ( const TriangleIndex& tri );

  // Are point index assigned?
  bool hasIndex() const;

  // Are points assigned?
  bool hasPoints() const;

  // Output triangle in text in PLY file
  std::string asPlyText() const;

  // Angle between the z-axis and the polygon normal in radians
  std::string slopeAngleStr() const;

  // Assigns vertices to triangle
  void assignPoints( std::vector<DirVector>& vertices );
};

#endif
