#include <Rcpp.h>
#include <regex>
#include <sstream>
#include "utils.h"

bool fileExists( const std::string& name )
{
  std::ifstream file( name.c_str() );
  return file.good();
}

std::vector<DirVector> readSTR( std::string strFile )
{
  DirVector point;
  bool hasReadAxisRecord = false;
  double x, y, z;
  std::regex pointPattern( "^\\d+\\s*,\\s*(\\d+\\.?\\d*)\\s*,\\s*(\\d+\\.?\\d*)\\s*,\\s*(-?\\d+\\.?\\d*).*\\s*" );
  std::smatch capturedPoint;
  std::string str;
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
