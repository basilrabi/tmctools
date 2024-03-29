#include <Rcpp.h>
#include <pqxx/pqxx>
#include <tuple>
#include "utils.h"

//' Write PLY to database
//'
//' Reads a PLY file and writes it to a PostGIS database via streaming.
//' The table will have the columns: \code{id integer} and
//' \code{geom geometry(PolygonZ, srid)}.
//'
//'
//' @param ply name of ply file
//' @param db_user PostGIS user
//' @param db_host PostGIS server host name or IP address
//' @param db_name database to write into
//' @param db_table table to write into. This will be overwritten if it exists!
//' @param schema schema to write into
//' @param port port number to connect to at the server host, or socket file
//'   name extension for Unix-domain connections
//' @param srid spatial reference identifier
//' @param drop_temporary_tables drop created tables after processing
//' @return void
//' @export
// [[Rcpp::export]]
void plyToDB( const std::string& ply,
              const std::string& db_user,
              const std::string& db_host,
              const std::string& db_name,
              const std::string& db_table,
              const std::string& schema = "public",
              const std::string& port = "5432",
              const std::string& srid = "3125",
              const bool& drop_temporary_tables = true )
{
  Rcpp::Environment tmctools = Rcpp::Environment::namespace_env( "tmctools" );
  Rcpp::Function psql = tmctools["psql"];
  long unsigned int i;
  std::string connectionParam = "postgresql://" + db_user + "@" + db_host + ":" + port + "/" + db_name + "?application_name=tmctools";
  std::string prefix = randomString( 5 );
  std::string sql;
  std::vector<DirVector> vertices;
  std::vector<TriangleIndex> faces;
  readPlyFile( ply, vertices, faces, false );

  sql = "DROP TABLE IF EXISTS " + schema + ".ply_vertices_text_" + prefix;
  sendQuery( connectionParam, sql );
  sql = "DROP TABLE IF EXISTS " + schema + ".ply_vertices_geom_" + prefix;
  sendQuery( connectionParam, sql );
  sql = "DROP TABLE IF EXISTS " + schema + ".ply_faces_" + prefix;
  sendQuery( connectionParam, sql );
  sql = "DROP TABLE IF EXISTS " + schema + "." + db_table;
  sendQuery( connectionParam, sql );

  // Prepare the temporary table containers.
  sql =
    "CREATE TABLE " + schema + ".ply_vertices_text_" + prefix + "(" +
    "  id integer PRIMARY KEY," +
    "  geom_text text NOT NULL" +
    ")";
  sendQuery( connectionParam, sql );
  sql =
    "CREATE TABLE " + schema + ".ply_vertices_geom_" + prefix + "(" +
    "  id integer PRIMARY KEY," +
    "  geom geometry(PointZ, " + srid + ") NOT NULL" +
    ")";
  sendQuery( connectionParam, sql );
  sql =
    "CREATE TABLE " + schema + ".ply_faces_" + prefix + "(" +
    "  id integer PRIMARY KEY," +
    "  p_a integer NOT NULL," +
    "  p_b integer NOT NULL," +
    "  p_c integer NOT NULL" +
    ")";
  sendQuery( connectionParam, sql );
  sql =
    "CREATE TABLE " + schema + "." + db_table + "(" +
    "  id integer PRIMARY KEY," +
    "  geom geometry(PolygonZ, " + srid + ") NOT NULL" +
    ")";
  sendQuery( connectionParam, sql );

  // Upload mesh data.
  pqxx::connection c{connectionParam};
  pqxx::work txn{c};
  pqxx::stream_to stream_v(pqxx::stream_to::table(
      txn,
      pqxx::table_path{schema, "ply_vertices_text_" + prefix},
      pqxx::table_path{"id", "geom_text"}
  ));
  for ( i = 1; i <= vertices.size(); i++ )
    stream_v << std::make_tuple( std::to_string( i ), vertices[i - 1].point() );
  stream_v.complete();
  pqxx::stream_to stream_f(pqxx::stream_to::table(
    txn,
    pqxx::table_path{schema, "ply_faces_" + prefix},
    pqxx::table_path{"id", "p_a", "p_b", "p_c"}
  ));
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
    "INSERT INTO " + schema + ".ply_vertices_geom_" + prefix + "(id, geom) " +
    "SELECT" +
    "  id," +
    "  ST_GeomFromText('POINTZ(' || geom_text || ')', " + srid + ") " +
    "FROM " + schema + ".ply_vertices_text_" + prefix;
  sendQuery( connectionParam, sql );
  sql = "VACUUM ANALYZE " + schema + ".ply_vertices_geom_" + prefix;
  psql( db_host, db_user, db_name, sql );
  sql =
    "INSERT INTO " + schema + "." + db_table + "(id, geom) " +
    "SELECT" +
    " faces.id," +
    " ST_MakePolygon(ST_MakeLine(ARRAY[p_a.geom, p_b.geom, p_c.geom, p_a.geom]))" +
    "FROM " + schema + ".ply_faces_" + prefix + " faces," +
    " " + schema + ".ply_vertices_geom_" + prefix + " p_a," +
    " " + schema + ".ply_vertices_geom_" + prefix + " p_b," +
    " " + schema + ".ply_vertices_geom_" + prefix + " p_c " +
    "WHERE faces.p_a = p_a.id AND faces.p_b = p_b.id AND faces.p_c = p_c.id";
  if ( drop_temporary_tables )
  {
    sendQuery( connectionParam, sql );
    sql = "DROP TABLE IF EXISTS " + schema + ".ply_vertices_text_" + prefix;
    sendQuery( connectionParam, sql );
    sql = "DROP TABLE IF EXISTS " + schema + ".ply_vertices_geom_" + prefix;
    sendQuery( connectionParam, sql );
    sql = "DROP TABLE IF EXISTS " + schema + ".ply_faces_" + prefix;
    sendQuery( connectionParam, sql );
  }
}
