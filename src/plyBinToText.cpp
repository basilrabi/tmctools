#include <Rcpp.h>
#include "utils.h"

//' Translate Binary PLY File
//'
//' Translates a binary PLY file to ASCII format.
//'
//' @param inPly name of ply file to be translated
//' @param outPly name of output ply file
//' @return void
//' @export
// [[Rcpp::export]]
void plyBinToText( const std::string& inPly,
                   const std::string& outPly = "text.ply" )
{
  std::vector<DirVector> vertices;
  std::vector<TriangleIndex> faces;
  readPly( inPly, vertices, faces );
  std::ofstream plyASCII;
  plyASCII.open( outPly );
  writePlyHeader( plyASCII, vertices.size(), faces.size() );
  for ( DirVector const& e : std::as_const( vertices ) )
    plyASCII << e.point() << "\n";
  for ( TriangleIndex const& e : std::as_const( faces ) )
    plyASCII << e.asPlyText() << "\n";
  plyASCII.close();
}
