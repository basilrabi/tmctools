#include <Rcpp.h>
#include <vector>
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
  Rcpp::StringVector polygon ( triangles.size() );
  Rcpp::NumericVector edgeLength ( triangles.size() );
  Rcpp::NumericVector slopeAngle ( triangles.size() );

  for ( unsigned int i = 0; i < triangles.size(); i++ )
  {
    // Check every n-lines for user interruption.
    if ( i % 1000 == 0 )
      Rcpp::checkUserInterrupt();

    if ( srid != "" )
      polygon[i] = triangles[i].ewkt( srid );
    else
      polygon[i] = triangles[i].wkt();
    edgeLength[i] = triangles[i].longestEdge();
    slopeAngle[i] = triangles[i].slopeAngle();
  }

  Rcpp::DataFrame out = Rcpp::DataFrame::create(
    Rcpp::Named( "polygon" ) = polygon,
    Rcpp::Named( "edge_length" ) = edgeLength,
    Rcpp::Named( "slope_angle" ) = slopeAngle,
    Rcpp::Named( "stringsAsFactors" ) = false
  );

  return out;
}
