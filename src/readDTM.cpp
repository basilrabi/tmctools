#include <Rcpp.h>
#include <fstream>
#include <regex>
#include <sstream>
#include <vector>
#include "dirVector.h"
#include "triangle.h"

bool fileExists( const std::string& name )
{
  std::ifstream file( name.c_str() );
  return file.good();
}

//' Read Surpac DTM file
//'
//' Reads a valid DTM file. It is assumed that both the DTM and String files
//'   are present in the same directory and have the same file name. Outputs
//'   a data.frane with each row representing a triangle of the DTM.
//'
//' @param dtmFile file name
//' @param srid spatial reference identifier
//' @return a data.frame with the following columns:
//'   \describe{
//'     \item{polygon}{polygon in ewkt or wkt format}
//'     \item{edge_length}{length of the longest edge}
//'   }
//' @export
// [[Rcpp::export]]
Rcpp::DataFrame readDTM( std::string dtmFile, std::string srid = "" )
{
  DirVector point;
  Triangle triangle;
  bool hasReadAxisRecord = false;
  double x, y, z;
  int i, p1, p2, p3;
  std::regex pointPattern( ".*\\d+\\s*,\\s*(\\d+\\.?\\d*)\\s*,\\s*(\\d+\\.?\\d*)\\s*,\\s*(-?\\d+\\.?\\d*).*\\s*" );
  std::regex trianglePattern( ".*\\d+\\s*,\\s*(\\d+)\\s*,\\s*(\\d+)\\s*,\\s*(\\d+)\\s*,\\s*\\d+\\s*,\\s*\\d+\\s*,\\s*\\d+.*\\s*" );
  std::smatch capturedPoint;
  std::string str;
  std::vector<DirVector> points;
  std::vector<Triangle> triangles;

  if ( !fileExists( dtmFile ) )
    Rcpp::stop( dtmFile + " does not exist!" );

  std::string stringFile = dtmFile.substr( 0, dtmFile.size() - 3 ) + "str";

  if ( !fileExists( stringFile ) )
    Rcpp::stop( stringFile + " does not exist!" );

  Rcpp::Rcout << "Reading string file...\n";
  std::ifstream fileString( stringFile );
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

  Rcpp::Rcout << "Reading dtm file...\n";
  i = 0;
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

  Rcpp::StringVector polygon ( triangles.size() );
  Rcpp::NumericVector edgeLength ( triangles.size() );

  for ( i = 0; i < triangles.size(); i++ )
  {
    // Check every n-lines for user interruption.
    if ( i % 1000 == 0 )
      Rcpp::checkUserInterrupt();

    if ( srid != "" )
    {
      polygon[i] = triangles[i].ewkt( srid );
    }
    else
    {
      polygon[i] = triangles[i].wkt();
    }
    edgeLength[i] = triangles[i].longestEdge();
  }

  Rcpp::DataFrame out = Rcpp::DataFrame::create(
    Rcpp::Named( "polygon" ) = polygon,
    Rcpp::Named( "edge_length" ) = edgeLength
  );

  return out;
}
