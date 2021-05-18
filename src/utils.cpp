#include <cstring>
#include <ctime>
#include <iomanip>
#include <iostream>
#include <pqxx/pqxx>
#include <regex>
#include <sstream>
#include <unistd.h>
#include "tinyply.h"
#include "utils.h"

std::smatch capturedPoint;
std::string str;

Rcpp::DataFrame triangleToDataFrame( std::vector<Triangle>& triangles,
                                     const std::string& srid )
{
  Rcpp::StringVector polygon ( triangles.size() );
  Rcpp::NumericVector edgeLength ( triangles.size() );
  Rcpp::NumericVector slopeAngle ( triangles.size() );

  for ( unsigned int i = 0; i < triangles.size(); i++ )
  {
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

bool fileExists( const std::string& name )
{
  std::ifstream file( name.c_str() );
  return file.good();
}

bool sendQuery ( const std::string& connectionParameters,
                 const std::string& query )
{
  pqxx::connection c{connectionParameters};
  pqxx::work txn{c};
  try
  {
    txn.exec( query );
    txn.commit();
  }
  catch ( std::exception const &e )
  {
    Rcpp::stop( e.what() );
    return false;
  }
  return true;
}

std::string randomString( const unsigned int& len )
{
  std::string tmpString;
  static const char alpha[] = "abcdefghijklmnopqrstuvwxyz";
  srand( ( unsigned ) time( NULL ) * getpid() );
  tmpString.reserve( len );

  for ( unsigned int i = 0; i < len; ++i )
    tmpString += alpha[rand() % ( sizeof( alpha ) - 1 )];

  return tmpString;
}

std::vector<DirVector> readSTR( const std::string& strFile )
{
  DirVector point;
  bool hasReadAxisRecord = false;
  double x, y, z;
  std::regex pointPattern( "^\\d+\\s*,\\s*(\\d+\\.?\\d*)\\s*,\\s*(\\d+\\.?\\d*)\\s*,\\s*(-?\\d+\\.?\\d*).*\\s*" );
  std::vector<DirVector> points;

  if ( !fileExists( strFile ) )
    Rcpp::stop( strFile + " does not exist!" );

  Rcpp::Rcout << "Reading string file...\n";
  std::ifstream fileString( strFile );
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

  return points;
}

std::vector<Triangle> pointToTri ( std::vector<DirVector>& points,
                                   const std::string& dtmFile )
{
  Triangle triangle;
  std::regex trianglePattern( "^\\d+\\s*,\\s*(\\d+)\\s*,\\s*(\\d+)\\s*,\\s*(\\d+)\\s*,\\s*\\d+\\s*,\\s*\\d+\\s*,\\s*\\d+.*\\s*" );
  std::vector<Triangle> triangles;
  unsigned int i = 0, p1, p2, p3;

  Rcpp::Rcout << "Reading dtm file...\n";
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

  return triangles;
}

unsigned int nrow( const std::string& connectionParameters,
                   const std::string& schema,
                   const std::string& table )
{
  pqxx::connection c{connectionParameters};
  pqxx::work txn{c};
  std::string query = "SELECT COUNT(*) FROM " + schema + "." + table;
  try
  {
    unsigned int rows = txn.query_value<unsigned int>( query );
    return rows;
  }
  catch ( std::exception const &e )
  {
    Rcpp::stop( e.what() );
    return 0;
  }
}

void readPly( const std::string& inPly,
              std::vector<DirVector>& outVertices,
              std::vector<TriangleIndex>& outFaces )
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
  if ( faces->t == tinyply::Type::INT16 )
  {
    std::vector<int1> faces1( faces->count );
    std::memcpy( faces1.data(), faces->buffer.get(), numFacesBytes );
    for ( i = 0; i < faces->count; i++ )
      outFaces.push_back( TriangleIndex( faces1[i].x, faces1[i].y, faces1[i].z ) );
  }
  else if ( faces->t == tinyply::Type::UINT16 )
  {
    std::vector<int2> faces2( faces->count );
    std::memcpy( faces2.data(), faces->buffer.get(), numFacesBytes );
    for ( i = 0; i < faces->count; i++ )
      outFaces.push_back( TriangleIndex( faces2[i].x, faces2[i].y, faces2[i].z ) );
  }
  else if ( faces->t == tinyply::Type::INT32 )
  {
    std::vector<int3> faces3( faces->count );
    std::memcpy( faces3.data(), faces->buffer.get(), numFacesBytes );
    for ( i = 0; i < faces->count; i++ )
      outFaces.push_back( TriangleIndex( faces3[i].x, faces3[i].y, faces3[i].z ) );
  }
  else if ( faces->t == tinyply::Type::UINT32 )
  {
    std::vector<int4> faces4( faces->count );
    std::memcpy( faces4.data(), faces->buffer.get(), numFacesBytes );
    for ( i = 0; i < faces->count; i++ )
      outFaces.push_back( TriangleIndex( faces4[i].x, faces4[i].y, faces4[i].z ) );
  }
  else
  {
    Rcpp::stop( "Unknown triangle type." );
  }

  // Copy vertices.
  const size_t numVerticesBytes = vertices->buffer.size_bytes();
  if ( vertices->t == tinyply::Type::FLOAT32 )
  {
    std::vector<float1> verts1( vertices->count );
    std::memcpy( verts1.data(), vertices->buffer.get(), numVerticesBytes );
    for ( i = 0; i < vertices->count; i++ )
      outVertices.push_back( DirVector( verts1[i].x, verts1[i].y, verts1[i].z ) );
  }
  else if ( vertices->t == tinyply::Type::FLOAT64 )
  {
    std::vector<float2> verts2( vertices->count );
    std::memcpy( verts2.data(), vertices->buffer.get(), numVerticesBytes );
    for ( i = 0; i < vertices->count; i++ )
      outVertices.push_back( DirVector( verts2[i].x, verts2[i].y, verts2[i].z ) );
  }
  else
  {
    Rcpp::stop( "Unknown vertex type." );
  }
}

void writePlyHeader( std::ofstream& plyFile,
                     const unsigned int& npoints,
                     const unsigned int& nfaces )
{
  plyFile << "ply\n"
          << "format ascii 1.0\n"
          << "comment TMC generated PLY File\n"
          << "element vertex " << npoints << "\n"
          << "property double x\n"
          << "property double y\n"
          << "property double z\n"
          << "element face " << nfaces << "\n"
          << "property list uchar int vertex_indices\n"
          << "end_header\n";
}

void writePlyHeaderFromDB( const std::string& plyFile,
                           const unsigned int& npoints,
                           const unsigned int& nfaces )
{
  std::ofstream ply;
  ply.open( plyFile );
  writePlyHeader( ply, npoints, nfaces );
  ply.close();
}

void writePlyFaceFromDB( const std::string& plyFile,
                         const std::string& connectionParameters,
                         const std::string& schemaPointSet,
                         const std::string& tablePointSet,
                         const std::string& schemaPoint,
                         const std::string& tablePoint )
{
  pqxx::connection c{connectionParameters};
  pqxx::work txn{c};
  std::string query =
    "WITH cte_a AS ("
    "  SELECT "
    "    pointset.tid,"
    "    pointset.pid,"
    "    points.id"
    "  FROM "+ schemaPointSet + "." + tablePointSet + " pointset" +
    "    INNER JOIN " + schemaPoint + "." + tablePoint + " points" +
    "      ON points.geom = pointset.geom" +
    ") " +
    "SELECT lat_a.id p_a, lat_b.id p_b, lat_c.id p_c " +
    "FROM cte_a lat_a, cte_a lat_b, cte_a lat_c " +
    "WHERE lat_a.tid = lat_b.tid" +
    "  AND lat_a.tid = lat_c.tid" +
    "  AND lat_a.pid = 1" +
    "  AND lat_b.pid = 2 "+
    "  AND lat_c.pid = 3";

  std::ofstream ply;
  ply.open( plyFile, std::ios_base::app );
  for ( auto [p_a, p_b, p_c] : txn.stream<unsigned int, unsigned int, unsigned int>( query ) )
  {
    ply << "3 "
        << std::to_string( p_a - 1 ) << " "
        << std::to_string( p_b - 1 ) << " "
        << std::to_string( p_c - 1 ) << "\n";
  }
  ply.close();
}

void writePlyVertexFromDB( const std::string& plyFile,
                           const std::string& connectionParameters,
                           const std::string& schema,
                           const std::string& table )
{
  pqxx::connection c{connectionParameters};
  pqxx::work txn{c};
  std::string query =
    "SELECT ST_X(geom) x, ST_Y(geom) y, ST_Z(geom) z "
    "FROM " + schema + "." + table + " " +
    "ORDER BY id";
  std::stringstream xx, yy, zz;

    std::ofstream ply;
    ply.open( plyFile, std::ios_base::app );
  for ( auto [x, y, z] : txn.stream<double, double, double>( query ) )
  {
    xx << std::fixed << std::setprecision( 3 ) << x;
    yy << std::fixed << std::setprecision( 3 ) << y;
    zz << std::fixed << std::setprecision( 3 ) << z;
    ply << xx.str() << " " << yy.str() << " " << zz.str() << "\n";
    xx.str( "" );
    yy.str( "" );
    zz.str( "" );
  }
  ply.close();
}
