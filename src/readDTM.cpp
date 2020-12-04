#include "utils.h"

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
//'     \item{slope_angle}{angle in radians between z-axis and the polygon normal}
//'   }
//' @export
// [[Rcpp::export]]
Rcpp::DataFrame readDTM( std::string dtmFile, std::string srid = "" )
{
  std::vector<DirVector> points;
  std::vector<Triangle> triangles;

  if ( !fileExists( dtmFile ) )
    Rcpp::stop( dtmFile + " does not exist!" );

  std::string stringFile = dtmFile.substr( 0, dtmFile.size() - 3 ) + "str";
  points = readSTR( stringFile );
  triangles = pointToTri( points, dtmFile );
  Rcpp::DataFrame out = triangleToDataFrame( triangles, srid );

  return out;
}
