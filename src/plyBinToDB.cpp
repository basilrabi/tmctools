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
  bool qa, qb;
  std::string connectionParam = "postgresql://" + user + "@" + hostname + ":" + port + "/" + dbname + "?application_name=tmctools";
  std::string prefix = randomString( 5 );
  std::string sql;
  std::vector<DirVector> vertices;
  std::vector<TriangleIndex> faces;
  readPly( inPly, vertices, faces );

  // TODO: Attempt to write to other schema in the future pqxx versions.
  sql = "DROP TABLE IF EXISTS plyupload" + prefix;
  sendQuery( connectionParam, sql );
  sql = "DROP TABLE IF EXISTS " + schema + "." + tableName;
  qa = sendQuery( connectionParam, sql );

  sql =
    "CREATE TABLE plyupload" + prefix + "(" +
    "  geom_text text NOT NULL," +
    "  slope_angle double precision NOT NULL" +
    ")";
  qb = sendQuery( connectionParam, sql );

  sql =
    "CREATE TABLE " + schema + "." + tableName + "(" +
    "  id SERIAL PRIMARY KEY," +
    "  geom geometry(PolygonZ, " + srid + ")," +
    "  slope_angle double precision NOT NULL" +
    ")";
  qb = sendQuery( connectionParam, sql );

  if ( qa && qb )
  {
    pqxx::connection c{connectionParam};
    pqxx::work txn{c};
    pqxx::stream_to stream{txn, "plyupload" + prefix,
                           std::vector<std::string>{"geom_text", "slope_angle"}};
    for ( TriangleIndex const &triangle: faces )
    {
      stream << std::make_tuple( triangle.wkt( vertices ), triangle.slopeAngleStr( vertices ) );
    }
    stream.complete();
    txn.commit();

    sql =
      "INSERT INTO " + schema + "." + tableName + "(geom, slope_angle)" +
      "SELECT ST_GeomFromText(geom_text, " + srid + ")," +
      "       slope_angle " +
      "FROM plyupload" + prefix;
    sendQuery( connectionParam, sql );
    sql = "DROP TABLE IF EXISTS plyupload" + prefix;
    sendQuery( connectionParam, sql );
  }
}
