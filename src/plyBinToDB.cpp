#include <Rcpp.h>
#include <pqxx/pqxx>
#include <tuple>
#include "utils.h"

//' Write binary PLY to database
//'
//' Reads the binary PLY file and writes it to a PostGIS database via streaming.
//' The table will have the columns: \code{geom geometry(PolygonZ, srid)} and
//' \code{slope_angle double precision}.
//'
//'
//' @param inPly name of ply file to be translated
//' @param user PostGIS user
//' @param hostname PostGIS server host name or IP address
//' @param dbname database to write into
//' @param tableName table to write into. This will be overwritten if it exists!
//' @param schema schema to write into
//' @param port port number to connect to at the server host, or socket file
//'   name extension for Unix-domain connections
//' @param srid spatial reference identifier
//' @return void
//' @export
// [[Rcpp::export]]
void plyBinToDB( const std::string& inPly,
                 const std::string& user,
                 const std::string& hostname,
                 const std::string& dbname,
                 const std::string& tableName,
                 const std::string& schema = "public",
                 const std::string& port = "5432",
                 const std::string& srid = "3125" )
{
  Rcpp::Environment tmctools = Rcpp::Environment::namespace_env( "tmctools" );
  Rcpp::Function psql = tmctools["psql"];
  long unsigned int i;
  std::string connectionParam = "postgresql://" + user + "@" + hostname + ":" + port + "/" + dbname + "?application_name=tmctools";
  std::string prefix = randomString( 5 );
  std::string sql;
  std::vector<DirVector> vertices;
  std::vector<TriangleIndex> faces;
  readPlyFile( inPly, vertices, faces, false );

  // TODO: Attempt to write to other schema in the future pqxx versions.
  sql = "DROP TABLE IF EXISTS ply_vertices_text_" + prefix;
  sendQuery( connectionParam, sql );
  sql = "DROP TABLE IF EXISTS ply_vertices_geom_" + prefix;
  sendQuery( connectionParam, sql );
  sql = "DROP TABLE IF EXISTS ply_faces_" + prefix;
  sendQuery( connectionParam, sql );
  sql = "DROP TABLE IF EXISTS " + schema + "." + tableName;
  sendQuery( connectionParam, sql );

  // Prepare the temporary table containers.
  sql =
    "CREATE TABLE ply_vertices_text_" + prefix + "(" +
    "  id integer PRIMARY KEY," +
    "  geom_text text NOT NULL" +
    ")";
  sendQuery( connectionParam, sql );
  sql =
    "CREATE TABLE ply_vertices_geom_" + prefix + "(" +
    "  id integer PRIMARY KEY," +
    "  geom geometry(PointZ, " + srid + ") NOT NULL" +
    ")";
  sendQuery( connectionParam, sql );
  sql =
    "CREATE TABLE ply_faces_" + prefix + "(" +
    "  id integer PRIMARY KEY," +
    "  p_a integer NOT NULL," +
    "  p_b integer NOT NULL," +
    "  p_c integer NOT NULL" +
    ")";
  sendQuery( connectionParam, sql );
  sql =
    "CREATE TABLE " + schema + "." + tableName + "(" +
    "  id integer PRIMARY KEY," +
    "  geom geometry(PolygonZ, " + srid + ") NOT NULL" +
    ")";
  sendQuery( connectionParam, sql );

  // Upload mesh data.
  pqxx::connection c{connectionParam};
  pqxx::work txn{c};
  pqxx::stream_to stream_v{
    txn,
    "ply_vertices_text_" + prefix,
    std::vector<std::string>{"id", "geom_text"}
  };
  for ( i = 1; i <= vertices.size(); i++ )
    stream_v << std::make_tuple( std::to_string( i ), vertices[i - 1].point() );
  stream_v.complete();
  pqxx::stream_to stream_f{
    txn,
    "ply_faces_" + prefix,
    std::vector<std::string>{"id", "p_a", "p_b", "p_c"}
  };
  for ( i = 1; i <= faces.size(); i++ )
  {
    stream_f << std::make_tuple(
        std::to_string( i ),
        std::to_string( faces[i - 1].ai + 1 ),
        std::to_string( faces[i - 1].bi + 1 ),
        std::to_string( faces[i - 1].ci + 1 )
    );
  }
  stream_f.complete();
  txn.commit();

  // Create polygon soup.
  sql =
    "INSERT INTO ply_vertices_geom_" + prefix + "(id, geom) " +
    "SELECT" +
    "  id," +
    "  ST_GeomFromText('POINTZ(' || geom_text || ')', " + srid + ") " +
    "FROM ply_vertices_text_" + prefix;
  sendQuery( connectionParam, sql );
  sql = "VACUUM ANALYZE ply_vertices_geom_" + prefix;
  psql( hostname, user, dbname, sql );
  sql =
    "INSERT INTO " + schema + "." + tableName + "(id, geom) " +
    "SELECT" +
    " faces.id," +
    " ST_MakePolygon(ST_MakeLine(ARRAY[p_a.geom, p_b.geom, p_c.geom, p_a.geom]))" +
    "FROM ply_faces_" + prefix + " faces," +
    "  ply_vertices_geom_" + prefix + " p_a," +
    "  ply_vertices_geom_" + prefix + " p_b," +
    "  ply_vertices_geom_" + prefix + " p_c " +
    "WHERE faces.p_a = p_a.id AND faces.p_b = p_b.id AND faces.p_c = p_c.id";
  sendQuery( connectionParam, sql );
  sql = "DROP TABLE IF EXISTS ply_vertices_text_" + prefix;
  sendQuery( connectionParam, sql );
  sql = "DROP TABLE IF EXISTS ply_vertices_geom_" + prefix;
  sendQuery( connectionParam, sql );
  sql = "DROP TABLE IF EXISTS ply_faces_" + prefix;
  sendQuery( connectionParam, sql );
}
