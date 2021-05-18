#include <Rcpp.h>
#include <cstring>
#include <iostream>
#include <fstream>
#include "tinyply.h"
#include "triangle.h"
#include "utils.h"

struct float3 { float x, y, z; };
struct int1 { int16_t x, y, z; };
struct int2 { uint16_t x, y, z; };
struct int3 { int32_t x, y, z; };

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
  std::ofstream plyASCII;
  std::shared_ptr<tinyply::PlyData> vertices, faces;
  std::unique_ptr<std::istream> file_stream;
  unsigned int i;

  try
  {
    file_stream.reset( new std::ifstream( inPly, std::ios::binary ) );
    if ( !file_stream || file_stream->fail() )
      throw std::runtime_error( "file_stream failed to open " + inPly );

    tinyply::PlyFile ply;
    ply.parse_header( *file_stream );

    try
    {
      faces = ply.request_properties_from_element( "face", { "vertex_indices" }, 3 );
      vertices = ply.request_properties_from_element( "vertex", { "x", "y", "z" } );
    }
    catch ( const std::exception & e )
    {
      std::cerr << "Caught exception: " << e.what() << std::endl;
    }
    ply.read( *file_stream );

  }
  catch ( const std::exception & e )
  {
    std::cerr << "Caught exception: " << e.what() << std::endl;
  }

  // Copy faces.
  const size_t numFacesBytes = faces->buffer.size_bytes();
  std::vector<TriangleIndex> triangle( faces->count );
  if ( faces->t == tinyply::Type::INT16 )
  {
    std::vector<int1> faces1( faces->count );
    std::memcpy( faces1.data(), faces->buffer.get(), numFacesBytes );
    for ( i = 0; i < faces->count; i++ )
      triangle[i] = TriangleIndex( faces1[i].x, faces1[i].y, faces1[i].z );
  }
  else if ( faces->t == tinyply::Type::UINT16 )
  {
    std::vector<int2> faces2( faces->count );
    std::memcpy( faces2.data(), faces->buffer.get(), numFacesBytes );
    for ( i = 0; i < faces->count; i++ )
      triangle[i] = TriangleIndex( faces2[i].x, faces2[i].y, faces2[i].z );
  }
  else if ( faces->t == tinyply::Type::INT32 )
  {
    std::vector<int3> faces3( faces->count );
    std::memcpy( faces3.data(), faces->buffer.get(), numFacesBytes );
    for ( i = 0; i < faces->count; i++ )
      triangle[i] = TriangleIndex( faces3[i].x, faces3[i].y, faces3[i].z );
  }
  else if ( faces->t == tinyply::Type::UINT32 )
  {
    std::memcpy( triangle.data(), faces->buffer.get(), numFacesBytes );
  }
  else
  {
    Rcpp::stop( "Unknown triangle type." );
  }

  // Copy vertices.
  const size_t numVerticesBytes = vertices->buffer.size_bytes();
  std::vector<DirVector> corners( vertices->count );
  if ( vertices->t == tinyply::Type::FLOAT32 )
  {
    std::vector<float3> verts( vertices->count );
    std::memcpy( verts.data(), vertices->buffer.get(), numVerticesBytes );
    for ( i = 0; i < vertices->count; i++ )
      corners[i] = DirVector( verts[i].x, verts[i].y, verts[i].z );
  }
  else if ( vertices->t == tinyply::Type::FLOAT64 )
  {
    std::memcpy( corners.data(), vertices->buffer.get(), numVerticesBytes );
  }
  else
  {
    Rcpp::stop( "Unknown vertex type." );
  }

  plyASCII.open( outPly );
  writePlyHeader( plyASCII, vertices->count, faces->count );
  for ( DirVector const& e : std::as_const( corners ) )
    plyASCII << e.point() << "\n";
  for ( TriangleIndex const& e : std::as_const( triangle ) )
    plyASCII << e.asPlyText() << "\n";
  plyASCII.close();
}
