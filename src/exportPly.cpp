#include <Rcpp.h>
#include "utils.h"

//' Export Surface to a PLY File
//'
//' A polygon soup represented by a PostGIS table with a PolygonZ column 'geom'
//'   is exported into a Stanford Polygon File (*.ply).
//'
//' @param user PostGIS user
//' @param hostname PostGIS server host name or IP address
//' @param dbname database containing the polygon soup
//' @param schema schema containing the polygon soup
//' @param tableName table containing the polygon soup
//' @param srid spatial reference identifier of the geometry
//' @param port port number to connect to at the server host, or socket file
//'   name extension for Unix-domain connections
//' @return file name of exported ply file
//' @export
// [[Rcpp::export]]
Rcpp::String exportPly( const std::string& user,
                        const std::string& hostname,
                        const std::string& dbname,
                        const std::string& schema,
                        const std::string& tableName,
                        const std::string& srid = "3125",
                        const std::string& port = "5432" )
{
  Rcpp::Environment tmctools = Rcpp::Environment::namespace_env( "tmctools" );
  Rcpp::Function psql = tmctools["psql"];
  std::string connectionParam = "postgresql://" + user + "@" + hostname + ":" + port + "/" + dbname + "?application_name=tmctools";
  std::string prefix = randomString( 5 );
  std::string sql;
  sql =
    "CREATE TABLE " + prefix + "_point(" +
    "  id SERIAL PRIMARY KEY," +
    "  geom geometry(PointZ," + srid + ") NOT NULL UNIQUE" +
    ")";
  sendQuery( connectionParam, sql );

  sql =
    "CREATE TABLE " + prefix + "_pointset(" +
    "  id SERIAL PRIMARY KEY," +
    "  tid integer NOT NULL REFERENCES " + schema + "." + tableName + ","
    "  pid integer NOT NULL," +
    "  geom geometry(PointZ," + srid + ") NOT NULL" +
    ")";
  sendQuery( connectionParam, sql );

  sql =
    "INSERT INTO " + prefix + "_pointset (tid, pid, geom) " +
    "WITH cte_a AS (" +
    "  SELECT" +
    "    id," +
    "    (ST_DumpPoints(geom)).path[2] pid," +
    "    (ST_DumpPoints(geom)).geom geom " +
    "  FROM " + schema + "." + tableName +
    ") " +
    "SELECT id, pid, geom FROM cte_a WHERE pid <= 3";
  sendQuery( connectionParam, sql );

  sql =
    "CREATE INDEX " + prefix + "_pointset_tpx " +
    "ON " + prefix + "_pointset (tid, pid) " +
    "WITH (fillfactor = 100)";
  sendQuery( connectionParam, sql );

  sql =
    "CREATE INDEX " + prefix + "_pointset_gix " +
    "ON " + prefix + "_pointset USING gist (geom) " +
    "WITH (fillfactor = 100)";
  sendQuery( connectionParam, sql );
  sql = "VACUUM ANALYZE " + prefix + "_pointset";
  psql( hostname, user, dbname, sql );

  sql =
    "INSERT INTO " + prefix + "_point (geom) " +
    "SELECT DISTINCT geom FROM " + prefix + "_pointset";
  sendQuery( connectionParam, sql );

  sql =
    "CREATE INDEX " + prefix + "_point_gix " +
    "ON " + prefix + "_point USING gist (geom) " +
    "WITH (fillfactor = 100)";
  sendQuery( connectionParam, sql );
  sql = "VACUUM ANALYZE " + prefix + "_point";
  psql( hostname, user, dbname, sql );

  writePlyHeaderFromDB( prefix + ".ply",
                        nrow( connectionParam, "public", prefix + "_point" ),
                        nrow( connectionParam, schema, tableName ) );
  writePlyVertexFromDB( prefix + ".ply",
                        connectionParam,
                        "public",
                        prefix + "_point" );
  writePlyFaceFromDB( prefix + ".ply",
                      connectionParam,
                      "public",
                      prefix + "_pointset",
                      "public",
                      prefix + "_point" );

  sendQuery( connectionParam, "DROP TABLE " + prefix + "_point" );
  sendQuery( connectionParam, "DROP TABLE " + prefix + "_pointset" );

  return prefix + ".ply";
}
