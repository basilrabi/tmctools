#include <iomanip>
#include <iostream>
#include <sstream>
#include "utils.h"

Triangle::Triangle()
{
  a = nullptr;
  b = nullptr;
  c = nullptr;
}

Triangle::Triangle( DirVector& x, DirVector& y, DirVector& z )
{
  a = &x;
  b = &y;
  c = &z;
}

Triangle& Triangle::operator=( const Triangle& tri )
{
  a = tri.a; b = tri.b; c = tri.c;
  return *this;
}

double Triangle::lengthA() const
{
  DirVector distance = *b - *c;
  return distance.magnitude();
}

double Triangle::lengthB() const
{
  DirVector distance = *a - *c;
  return distance.magnitude();
}

double Triangle::lengthC() const
{
  DirVector distance = *a - *b;
  return distance.magnitude();
}

double Triangle::longestEdge() const
{
  double length = lengthA();

  if ( length < lengthB() )
    length = lengthB();

  if ( length < lengthC() )
    length = lengthC();

  return length;
}

double Triangle::slopeAngle() const
{
  DirVector sideA = *b - *a;
  DirVector sideB = *c - *a;
  return slopeAngleBase( sideA, sideB );
}

std::string Triangle::asText() const
{
  std::string text;
  text = "((" +
    a->point() + "," +
    b->point() + "," +
    c->point() + "," +
    a->point() + "))";

  return text;
}

std::string Triangle::ewkt( const std::string& srid ) const
{
  std::string text;
  text = "SRID=" + srid + ";POLYGON" + asText();
  return text;
}

std::string Triangle::wkt() const
{
  std::string text;
  text = "POLYGON Z " + asText();
  return text;
}



TriangleIndex::TriangleIndex()
{
  a = 0;
  b = 0;
  c = 0;
}

TriangleIndex::TriangleIndex( const uint32_t& x, const uint32_t& y, const uint32_t& z)
{
  a = x;
  b = y;
  c = z;
}

TriangleIndex& TriangleIndex::operator=( const TriangleIndex& tri )
{
  a = tri.a; b = tri.b; c = tri.c;
  return *this;
}

bool TriangleIndex::hasPoints() const
{
  return ( a + b + c > 0 ) && ( a != b ) && ( b != c );
}

double TriangleIndex::slopeAngle( const std::vector<DirVector>& points ) const
{
  DirVector sideA = points[b] - points[a];
  DirVector sideB = points[c] - points[a];
  return slopeAngleBase( sideA, sideB );
}

std::string TriangleIndex::asPlyText() const
{
  std::string text;

  if ( !( hasPoints() ) )
    return text;

  text = "3 " +
    std::to_string( a ) + " " +
    std::to_string( b ) + " " +
    std::to_string( c );

  return text;
}

std::string TriangleIndex::asText( const std::vector<DirVector>& points ) const
{
  std::string text;

  if ( !( hasPoints() ) )
    return text;

  text = "((" +
    points[a].point() + "," +
    points[b].point() + "," +
    points[c].point() + "," +
    points[a].point() + "))";

  return text;
}

std::string TriangleIndex::slopeAngleStr( const std::vector<DirVector>& points ) const
{
  std::stringstream slope;
  slope << std::fixed << std::setprecision( 16 ) << slopeAngle( points );
  return slope.str();
}

std::string TriangleIndex::wkt( const std::vector<DirVector>& points ) const
{
  std::string text;

  if ( !( hasPoints() ) )
    return text;

  text = "POLYGON Z " + this->asText( points );
  return text;
}
