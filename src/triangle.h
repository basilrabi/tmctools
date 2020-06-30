#ifndef TRIANGLE_H
#define TRIANGLE_H

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
  double longestEdge();

  // Output triangle in extended well-known text
  std::string ewkt( const std::string& srid );

  // Output triangle in well-known text
  std::string wkt();

private:

  // length of edge opposite to vertex a
  double lengthA();

  // length of edge opposite to vertex b
  double lengthB();

  // length of edge opposite to vertex c
  double lengthC();

  // Output triangle in text
  std::string asText();
};

#endif
