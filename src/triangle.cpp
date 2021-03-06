#include <cmath>
#include <string>
#include "dirVector.h"
#include "triangle.h"

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

double Triangle::lengthA()
{
  DirVector distance = *b - *c;
  return distance.magnitude();
}

double Triangle::lengthB()
{
  DirVector distance = *a - *c;
  return distance.magnitude();
}

double Triangle::lengthC()
{
  DirVector distance = *a - *b;
  return distance.magnitude();
}

double Triangle::longestEdge()
{
  double length = lengthA();

  if ( length < lengthB() )
    length = lengthB();

  if ( length < lengthC() )
    length = lengthC();

  return length;
}

double Triangle::slopeAngle()
{
  DirVector sideA = *b - *a;
  DirVector sideB = *c - *a;
  DirVector normal = sideA * sideB;
  DirVector z ( 0, 0, 1 );
  double angle = angle_between( z, normal );
  if ( angle > M_PI_2 )
    angle = M_PI - angle;
  return angle;
}

std::string Triangle::asText()
{
  std::string text;
  text = "((" +
    a->point() + "," +
    b->point() + "," +
    c->point() + "," +
    a->point() + "))";

  return text;
}

std::string Triangle::ewkt( const std::string& srid )
{
  std::string text;
  text = "SRID=" + srid + ";POLYGON" + asText();
  return text;
}

std::string Triangle::wkt()
{
  std::string text;
  text = "POLYGON Z " + asText();
  return text;
}
