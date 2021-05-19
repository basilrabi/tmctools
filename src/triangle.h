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

  // The length of the longest edge
  double longestEdge() const;

  // Angle between the z-axis and the polygon normal in radians
  double slopeAngle() const;

  // Output triangle in extended well-known text
  std::string ewkt( const std::string& srid ) const;

  // Output triangle in well-known text
  std::string wkt() const;

private:

  // length of edge opposite to vertex a
  double lengthA() const;

  // length of edge opposite to vertex b
  double lengthB() const;

  // length of edge opposite to vertex c
  double lengthC() const;

  // Output triangle in text
  std::string asText() const;
};


// triangle in a ply file
class TriangleIndex
{
public:

  // Vertices of the triangle
  uint32_t a, b, c;

  // Constructor
  TriangleIndex();
  TriangleIndex( const uint32_t&, const uint32_t&, const uint32_t& );

  // Desctructor
  ~TriangleIndex(){}

  // Copy Constructor
  TriangleIndex& operator= ( const TriangleIndex& tri );

  // Are points assigned?
  bool hasPoints() const;

  // Angle between the z-axis and the polygon normal in radians
  double slopeAngle( const std::vector<DirVector>& points ) const;

  // Output triangle in text in PLY file
  std::string asPlyText() const;

  // Output triangle in text
  std::string asText( const std::vector<DirVector>& points ) const;

  // Output triangle in well-known text
  std::string wkt( const std::vector<DirVector>& points ) const;
};

#endif
