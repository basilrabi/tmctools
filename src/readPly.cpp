#include <fstream>
#include <regex>
#include <sstream>
#include "utils.h"

//' Read PLY File
//'
//' Reads the ply an covert it to a data frame.
//'
//' @param plyFile file name
//' @param srid spatial reference identifier of the geometry if output is ewkt
//' @return a data.frame with the following columns:
//'   \describe{
//'     \item{area_2d}{2D area of the polygon}
//'     \item{edge_length}{length of the longest edge}
//'     \item{slope_angle}{angle in radians between z-axis and the polygon normal}
//'     \item{polygon}{polygon in ewkt or wkt format}
//'   }
//' @export
// [[Rcpp::export]]
Rcpp::DataFrame readPly( const std::string& plyFile, const std::string& srid = "" )
{
  std::vector<DirVector> vertices;
  std::vector<TriangleIndex> triangles;
  readPlyFile( plyFile, vertices, triangles, true );
  Rcpp::DataFrame out = triangleToDataFrame( triangles, srid );
  return out;
}
