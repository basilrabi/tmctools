#include "utils.h"

//' Read Surpac DTM file
//'
//' Reads a valid DTM file. It is assumed that both the DTM and String files
//'   are present in the same directory and have the same file name. Outputs
//'   a data.frame with each row representing a triangle of the DTM.
//'
//' @param dtm file name
//' @param srid spatial reference identifier
//' @return a data.frame with the following columns:
//'   \describe{
//'     \item{area_2d}{2D area of the polygon}
//'     \item{edge_length}{length of the longest edge}
//'     \item{slope_angle}{angle in radians between z-axis and the polygon normal}
//'     \item{polygon}{polygon in ewkt or wkt format}
//'   }
//' @export
// [[Rcpp::export]]
Rcpp::DataFrame readDTM( std::string dtm, std::string srid = "" )
{
  std::vector<DirVector> points;
  std::vector<Triangle> triangles;

  if ( !fileExists( dtm ) )
    Rcpp::stop( dtm + " does not exist!" );

  std::string stringFile = dtm.substr( 0, dtm.size() - 3 ) + "str";
  points = readSTR( stringFile );
  triangles = pointToTri( points, dtm );
  Rcpp::DataFrame out = triangleToDataFrame( triangles, srid );

  return out;
}
