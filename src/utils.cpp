#include <Rcpp.h>
#include <regex>
#include <sstream>
#include "utils.h"

std::smatch capturedPoint;
std::string str;

bool fileExists( const std::string& name )
{
  std::ifstream file( name.c_str() );
  return file.good();
}

std::vector<DirVector> readSTR( const std::string& strFile )
{
  DirVector point;
  bool hasReadAxisRecord = false;
  double x, y, z;
  std::regex pointPattern( "^\\d+\\s*,\\s*(\\d+\\.?\\d*)\\s*,\\s*(\\d+\\.?\\d*)\\s*,\\s*(-?\\d+\\.?\\d*).*\\s*" );
  std::vector<DirVector> points;

  if ( !fileExists( strFile ) )
    Rcpp::stop( strFile + " does not exist!" );

  Rcpp::Rcout << "Reading string file...\n";
  std::ifstream fileString( strFile );
  while ( std::getline( fileString, str ) )
  {
    if ( std::regex_match( str, capturedPoint, pointPattern ) )
    {
      if ( hasReadAxisRecord )
      {
        std::stringstream xx( capturedPoint.str( 2 ) );
        std::stringstream yy( capturedPoint.str( 1 ) );
        std::stringstream zz( capturedPoint.str( 3 ) );
        xx >> x;
        yy >> y;
        zz >> z;
        point = DirVector( x, y, z );
        points.push_back( point );
      }
      else
      {
        hasReadAxisRecord = true;
      }
    }
  }

  return points;
}

std::vector<Triangle> pointToTri ( std::vector<DirVector>& points,
                                   const std::string& dtmFile )
{
  Triangle triangle;
  std::regex trianglePattern( "^\\d+\\s*,\\s*(\\d+)\\s*,\\s*(\\d+)\\s*,\\s*(\\d+)\\s*,\\s*\\d+\\s*,\\s*\\d+\\s*,\\s*\\d+.*\\s*" );
  std::vector<Triangle> triangles;
  unsigned int i = 0, p1, p2, p3;

  Rcpp::Rcout << "Reading dtm file...\n";
  std::ifstream fileDtm( dtmFile );
  while ( std::getline( fileDtm, str ) )
  {
    // Check every n-lines for user interruption.
    if ( i == 1000 )
    {
      Rcpp::checkUserInterrupt();
      i = 0;
    }
    i++;

    if ( std::regex_match( str, capturedPoint, trianglePattern ) )
    {
      std::stringstream e1( capturedPoint.str( 1 ) );
      std::stringstream e2( capturedPoint.str( 2 ) );
      std::stringstream e3( capturedPoint.str( 3 ) );
      e1 >> p1;
      e2 >> p2;
      e3 >> p3;
      triangle = Triangle( points[p1 - 1], points[p2 - 1], points[p3 - 1] );
      triangles.push_back( triangle );
    }
  }

  return triangles;
}
