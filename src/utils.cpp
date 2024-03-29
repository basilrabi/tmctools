#include <cmath>
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

bool fileExists( const std::string& name )
{
  std::ifstream file( name.c_str() );
  return file.good();
}

bool sendQuery ( const std::string& connection_parameters,
                 const std::string& query )
{
  pqxx::connection c{connection_parameters};
  pqxx::work txn{c};
  try
  {
    txn.exec( query );
    txn.commit();
    return true;
  }
  catch ( std::exception const &e )
  {
    Rcpp::stop( e.what() );
    return false;
  }
}

double slopeAngleBase( const DirVector& a, const DirVector& b)
{
  DirVector normal = a * b;
  DirVector z ( 0, 0, 1 );
  double angle = angle_between( z, normal );
  if ( angle > M_PI_2 )
    angle = M_PI - angle;
  return angle;
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

std::vector<DirVector> readSTR( const std::string& str_file )
{
  DirVector point;
  bool hasReadAxisRecord = false;
  double x, y, z;
  std::regex pointPattern( "^\\d+\\s*,\\s*(\\d+\\.?\\d*)\\s*,\\s*(\\d+\\.?\\d*)\\s*,\\s*(-?\\d+\\.?\\d*).*\\s*" );
  std::vector<DirVector> points;

  if ( !fileExists( str_file ) )
    Rcpp::stop( str_file + " does not exist!" );

  Rcpp::Rcout << "Reading string file...\n";
  std::ifstream fileString( str_file );
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
                                   const std::string& dtm )
{
  Triangle triangle;
  std::regex trianglePattern( "^\\d+\\s*,\\s*(\\d+)\\s*,\\s*(\\d+)\\s*,\\s*(\\d+)\\s*,\\s*\\d+\\s*,\\s*\\d+\\s*,\\s*\\d+.*\\s*" );
  std::vector<Triangle> triangles;
  unsigned int i = 0, p1, p2, p3;

  Rcpp::Rcout << "Reading dtm file...\n";
  std::ifstream dtm_file( dtm );
  while ( std::getline( dtm_file, str ) )
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

unsigned int nrow( const std::string& connection_parameters,
                   const std::string& schema,
                   const std::string& table )
{
  pqxx::connection c{connection_parameters};
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

void readPlyFile( const std::string& in_ply,
                  std::vector<DirVector>& out_vertices,
                  std::vector<TriangleIndex>& out_faces,
                  bool assign_triangle )
{
  std::ofstream ply_ascii;
  std::shared_ptr<tinyply::PlyData> vertices, faces;
  std::unique_ptr<std::istream> file_stream;
  unsigned int i;

  try
  {
    file_stream.reset( new std::ifstream( in_ply, std::ios::binary ) );
    if ( !file_stream || file_stream->fail() )
      Rcpp::stop( "Failed to open " + in_ply );

    tinyply::PlyFile ply;
    ply.parse_header( *file_stream );

    try
    {
      faces = ply.request_properties_from_element( "face", { "vertex_indices" }, 3 );
      vertices = ply.request_properties_from_element( "vertex", { "x", "y", "z" } );
    }
    catch ( const std::exception & e )
    {
      Rcpp::stop( e.what() );
    }
    ply.read( *file_stream );

  }
  catch ( const std::exception & e )
  {
    Rcpp::stop( e.what() );
  }

  // Copy faces.
  const size_t num_faces_bytes = faces->buffer.size_bytes();
  if ( faces->t == tinyply::Type::INVALID )
  {
    Rcpp::stop( "Invalid PLY." );
  }
  else if ( faces->t == tinyply::Type::INT8 )
  {
    std::vector<int5> faces1( faces->count );
    std::memcpy( faces1.data(), faces->buffer.get(), num_faces_bytes );
    for ( i = 0; i < faces->count; i++ )
      out_faces.push_back( TriangleIndex( faces1[i].x, faces1[i].y, faces1[i].z ) );
  }
  else if ( faces->t == tinyply::Type::UINT8 )
  {
    std::vector<int6> faces1( faces->count );
    std::memcpy( faces1.data(), faces->buffer.get(), num_faces_bytes );
    for ( i = 0; i < faces->count; i++ )
      out_faces.push_back( TriangleIndex( faces1[i].x, faces1[i].y, faces1[i].z ) );
  }
  else if ( faces->t == tinyply::Type::INT16 )
  {
    std::vector<int1> faces1( faces->count );
    std::memcpy( faces1.data(), faces->buffer.get(), num_faces_bytes );
    for ( i = 0; i < faces->count; i++ )
      out_faces.push_back( TriangleIndex( faces1[i].x, faces1[i].y, faces1[i].z ) );
  }
  else if ( faces->t == tinyply::Type::UINT16 )
  {
    std::vector<int2> faces2( faces->count );
    std::memcpy( faces2.data(), faces->buffer.get(), num_faces_bytes );
    for ( i = 0; i < faces->count; i++ )
      out_faces.push_back( TriangleIndex( faces2[i].x, faces2[i].y, faces2[i].z ) );
  }
  else if ( faces->t == tinyply::Type::INT32 )
  {
    std::vector<int3> faces3( faces->count );
    std::memcpy( faces3.data(), faces->buffer.get(), num_faces_bytes );
    for ( i = 0; i < faces->count; i++ )
      out_faces.push_back( TriangleIndex( faces3[i].x, faces3[i].y, faces3[i].z ) );
  }
  else if ( faces->t == tinyply::Type::UINT32 )
  {
    std::vector<int4> faces4( faces->count );
    std::memcpy( faces4.data(), faces->buffer.get(), num_faces_bytes );
    for ( i = 0; i < faces->count; i++ )
      out_faces.push_back( TriangleIndex( faces4[i].x, faces4[i].y, faces4[i].z ) );
  }
  else
  {
    Rcpp::stop( "Unknown triangle type." );
  }

  // Copy vertices.
  const size_t num_vertices_bytes = vertices->buffer.size_bytes();
  if ( vertices->t == tinyply::Type::FLOAT32 )
  {
    std::vector<float1> verts1( vertices->count );
    std::memcpy( verts1.data(), vertices->buffer.get(), num_vertices_bytes );
    for ( i = 0; i < vertices->count; i++ )
      out_vertices.push_back( DirVector( verts1[i].x, verts1[i].y, verts1[i].z ) );
  }
  else if ( vertices->t == tinyply::Type::FLOAT64 )
  {
    std::vector<float2> verts2( vertices->count );
    std::memcpy( verts2.data(), vertices->buffer.get(), num_vertices_bytes );
    for ( i = 0; i < vertices->count; i++ )
      out_vertices.push_back( DirVector( verts2[i].x, verts2[i].y, verts2[i].z ) );
  }
  else
  {
    Rcpp::stop( "Unknown vertex type." );
  }

  if ( assign_triangle )
  {
    for ( TriangleIndex &triangle: out_faces )
      triangle.assignPoints( out_vertices );
  }
}

void writePlyBinFromDB( const std::string& prefix,
                        const std::string& connection_parameters)
{
  std::filebuf ply_buffer;
  std::string query;
  std::string ply = prefix + ".ply";
  std::vector<float2> vertices;
  std::vector<int4> triangles;
  tinyply::PlyFile ply_file;

  pqxx::connection c{connection_parameters};
  pqxx::work txn{c};
  query =
    "SELECT ST_X(geom) x, ST_Y(geom) y, ST_Z(geom) z "
    "FROM " + prefix +  "_point "
    "ORDER BY id";

  for ( auto [x, y, z] : txn.stream<double, double, double>( query ) )
    vertices.push_back( float2{x, y, z} );

  query =
    "WITH cte_a AS ("
    "  SELECT "
    "    pointset.tid,"
    "    pointset.pid,"
    "    points.id"
    "  FROM "+ prefix + "_pointset pointset"
    "    INNER JOIN " + prefix + "_point points"
    "      ON points.geom = pointset.geom"
    ") "
    "SELECT lat_a.id p_a, lat_b.id p_b, lat_c.id p_c "
    "FROM cte_a lat_a, cte_a lat_b, cte_a lat_c "
    "WHERE lat_a.tid = lat_b.tid"
    "  AND lat_a.tid = lat_c.tid"
    "  AND lat_a.pid = 1"
    "  AND lat_b.pid = 2"
    "  AND lat_c.pid = 3";

  for ( auto [i, j, k] : txn.stream<uint32_t, uint32_t, uint32_t>( query ) )
    triangles.push_back( int4{i - 1, j - 1, k - 1} );

  ply_buffer.open( ply, std::ios::out | std::ios::binary );
  std::ostream ply_stream( &ply_buffer );
  if ( ply_stream.fail() )
    Rcpp::stop( "Failed to open " + ply + "." );

  ply_file.add_properties_to_element( "vertex",
                                      {"x", "y", "z"},
                                      tinyply::Type::FLOAT64,
                                      vertices.size(),
                                      reinterpret_cast<uint8_t*>( vertices.data() ),
                                      tinyply::Type::INVALID,
                                      0 );
  ply_file.add_properties_to_element( "face",
                                      {"vertex_indices"},
                                      tinyply::Type::UINT32,
                                      triangles.size(),
                                      reinterpret_cast<uint8_t*>( triangles.data() ),
                                      tinyply::Type::UINT32,
                                      3 );
  ply_file.get_comments().push_back( "TMC generated PLY File" );
  ply_file.write( ply_stream, true );
}

void writePlyHeader( std::ofstream& ply,
                     const unsigned int& n_points,
                     const unsigned int& n_faces )
{
  ply << "ply\n"
      << "format ascii 1.0\n"
      << "comment TMC generated PLY File\n"
      << "element vertex " << n_points << "\n"
      << "property double x\n"
      << "property double y\n"
      << "property double z\n"
      << "element face " << n_faces << "\n"
      << "property list uchar int vertex_indices\n"
      << "end_header\n";
}

void writePlyHeaderFromDB( const std::string& ply,
                           const unsigned int& n_points,
                           const unsigned int& n_faces )
{
  std::ofstream ply_stream;
  ply_stream.open( ply );
  writePlyHeader( ply_stream, n_points, n_faces );
  ply_stream.close();
}

void writePlyFaceFromDB( const std::string& connection_parameters,
                         const std::string& prefix )
{
  pqxx::connection c{connection_parameters};
  pqxx::work txn{c};
  std::string query =
    "WITH cte_a AS ("
    "  SELECT "
    "    pointset.tid,"
    "    pointset.pid,"
    "    points.id"
    "  FROM " + prefix + "_pointset pointset" +
    "    INNER JOIN " + prefix + "_point points" +
    "      ON points.geom = pointset.geom" +
    ") " +
    "SELECT lat_a.id p_a, lat_b.id p_b, lat_c.id p_c " +
    "FROM cte_a lat_a, cte_a lat_b, cte_a lat_c " +
    "WHERE lat_a.tid = lat_b.tid" +
    "  AND lat_a.tid = lat_c.tid" +
    "  AND lat_a.pid = 1" +
    "  AND lat_b.pid = 2"+
    "  AND lat_c.pid = 3";

  std::ofstream ply_stream;
  ply_stream.open( prefix + ".ply", std::ios_base::app );
  for ( auto [p_a, p_b, p_c] : txn.stream<unsigned int, unsigned int, unsigned int>( query ) )
  {
    ply_stream << "3 "
               << std::to_string( p_a - 1 ) << " "
               << std::to_string( p_b - 1 ) << " "
               << std::to_string( p_c - 1 ) << "\n";
  }
  ply_stream.close();
}

void writePlyVertexFromDB( const std::string& connection_parameters,
                           const std::string& prefix,
                           const unsigned int& digits )
{
  pqxx::connection c{connection_parameters};
  pqxx::work txn{c};
  std::string query =
    "SELECT ST_X(geom) x, ST_Y(geom) y, ST_Z(geom) z "
    "FROM " + prefix + "_point " +
    "ORDER BY id";
  std::stringstream xx, yy, zz;

  std::ofstream ply_stream;
  ply_stream.open( prefix + ".ply", std::ios_base::app );
  for ( auto [x, y, z] : txn.stream<double, double, double>( query ) )
  {
    xx << std::fixed << std::setprecision( digits ) << x;
    yy << std::fixed << std::setprecision( digits ) << y;
    zz << std::fixed << std::setprecision( digits ) << z;
    ply_stream << xx.str() << " " << yy.str() << " " << zz.str() << "\n";
    xx.str( "" );
    yy.str( "" );
    zz.str( "" );
  }
  ply_stream.close();
}
