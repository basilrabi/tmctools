#include <fstream>
#include <regex>
#include <sstream>
#include "utils.h"

//' Read PLY File
//'
//' Reads the ply an covert it to a data frame.
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
Rcpp::DataFrame readPly( const std::string& plyFile )
{
  std::vector<DirVector> vertices;
  std::vector<TriangleIndex> triangles;
  readPly( plyFile, vertices, triangles );
  Rcpp::DataFrame out = triangleToDataFrame( triangles, vertices, "" );
  return out;
}
