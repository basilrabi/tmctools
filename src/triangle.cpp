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

double Triangle::area2d() const
{
  double half{0.5};
  DirVector sa, sb, sc;
  sa = b->_2d() - a->_2d();
  sb = c->_2d() - a->_2d();
  sc = sa * sb;
  return half * sc.magnitude();
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
  a = nullptr;
  b = nullptr;
  c = nullptr;
  ai = 0;
  bi = 0;
  ci = 0;
}

TriangleIndex::TriangleIndex( const uint32_t& x, const uint32_t& y, const uint32_t& z)
{
  a = nullptr;
  b = nullptr;
  c = nullptr;
  ai = x;
  bi = y;
  ci = z;
}

TriangleIndex& TriangleIndex::operator=( const TriangleIndex& tri )
{
  a = tri.a;
  b = tri.b;
  c = tri.c;
  ai = tri.ai;
  bi = tri.bi;
  ci = tri.ci;
  return *this;
}

bool TriangleIndex::hasIndex() const
{
  return ( ai + bi + ci > 0 ) && ( ai != bi ) && ( bi != ci );
}

bool TriangleIndex::hasPoints() const
{
  return hasIndex() &&
    ( a != nullptr ) &&
    ( b != nullptr ) &&
    ( c != nullptr );
}

std::string TriangleIndex::asPlyText() const
{
  std::string text;

  if ( !( hasPoints() ) )
    return text;

  text = "3 " +
    std::to_string( ai ) + " " +
    std::to_string( bi ) + " " +
    std::to_string( ci );

  return text;
}

std::string TriangleIndex::slopeAngleStr() const
{
  std::stringstream slope;
  slope << std::fixed << std::setprecision( 16 ) << slopeAngle();
  return slope.str();
}

void TriangleIndex::assignPoints( std::vector<DirVector>& vertices )
{
  a = &( vertices.at( ai ) );
  b = &( vertices.at( bi ) );
  c = &( vertices.at( ci ) );
}
