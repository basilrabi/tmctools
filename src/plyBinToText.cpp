#include <Rcpp.h>
#include "utils.h"

//' Translate Binary PLY File
//'
//' Translates a binary PLY file to ASCII format.
//'
//' @param in_ply name of ply file to be translated
//' @param out_ply name of output ply file
//' @return void
//' @export
// [[Rcpp::export]]
void plyBinToText( const std::string& in_ply,
                   const std::string& out_ply = "text.ply" )
{
  std::vector<DirVector> vertices;
  std::vector<TriangleIndex> faces;
  readPlyFile( in_ply, vertices, faces, true );
  std::ofstream ply_ascii;
  ply_ascii.open( out_ply );
  writePlyHeader( ply_ascii, vertices.size(), faces.size() );
  for ( DirVector const& e : std::as_const( vertices ) )
    ply_ascii << e.point() << "\n";
  for ( TriangleIndex const& e : std::as_const( faces ) )
    ply_ascii << e.asPlyText() << "\n";
  ply_ascii.close();
}
