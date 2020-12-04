#include <regex>
#include <sstream>
#include "utils.h"

//' Read PLY File
//'
//' Reads the ply file created in \link{updateInsituSurface}.
//'
//' @param plyFile file name
//' @return a data.frame with the following columns:
//'   \describe{
//'     \item{polygon}{polygon in ewkt or wkt format}
//'     \item{edge_length}{length of the longest edge}
//'     \item{slope_angle}{angle in radians between z-axis and the polygon normal}
//'   }
//' @export
// [[Rcpp::export]]
Rcpp::DataFrame readPlyText( std::string plyFile )
{
  DirVector point;
  Triangle triangle;
  double x, y, z;
  int i, j, k;
  std::regex pointPattern( "^(\\d+\\.?\\d*)\\s*(\\d+\\.?\\d*)\\s*(-?\\d+\\.?\\d*)\\s*" );
  std::regex trianglePattern( "^3\\s*(\\d+)\\s*(\\d+)\\s*(\\d+)\\s*" );
  std::smatch capturedPoint;
  std::smatch capturedTriangle;
  std::string str;
  std::vector<DirVector> points;
  std::vector<Triangle> triangles;

  if ( !fileExists( plyFile ) )
    Rcpp::stop( plyFile + " does not exist!" );

  std::ifstream fileString( plyFile );
  while ( std::getline( fileString, str ) )
  {
    Rcpp::Rcout << str << "\n";
    if ( std::regex_match( str, capturedPoint, pointPattern ) )
    {
      std::stringstream xx( capturedPoint.str( 1 ) );
      std::stringstream yy( capturedPoint.str( 2 ) );
      std::stringstream zz( capturedPoint.str( 3 ) );
      xx >> x;
      yy >> y;
      zz >> z;
      point = DirVector( x, y, z );
      points.push_back( point );
    }
    else if ( std::regex_match( str, capturedTriangle, trianglePattern ) )
    {
      std::stringstream xx( capturedTriangle.str( 1 ) );
      std::stringstream yy( capturedTriangle.str( 2 ) );
      std::stringstream zz( capturedTriangle.str( 3 ) );
      xx >> i;
      yy >> j;
      zz >> k;
      triangle = Triangle( points[i], points[j], points[k] );
      triangles.push_back( triangle );
    }
  }
  Rcpp::DataFrame out = triangleToDataFrame( triangles, "" );

  return out;
}
