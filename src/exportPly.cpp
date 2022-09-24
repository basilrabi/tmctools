#include <Rcpp.h>
#include "utils.h"

//' Export Surface to a PLY File
//'
//' A polygon soup represented by a PostGIS table with a PolygonZ column 'geom'
//'   is exported into a Stanford Polygon File (*.ply). During exportation, a
//'   table will be created in the default schema. Thus, if the user has no
//'   privilege to write on the default schema, exporting will fail.
//'
//' @param db_user PostGIS user
//' @param db_host PostGIS server host name or IP address
//' @param db_name database containing the polygon soup
//' @param schema schema containing the polygon soup
//' @param db_table table containing the polygon soup
//' @param srid spatial reference identifier of the geometry
//' @param port port number to connect to at the server host, or socket file
//'   name extension for Unix-domain connections
//' @param digits precision in number of decimal digits
//' @param binary output binary file or not (ASCII)
//' @return file name of exported ply file
//' @param drop_temporary_tables drop created tables after processing
//' @export
// [[Rcpp::export]]
Rcpp::String exportPly( const std::string& db_user,
                        const std::string& db_host,
                        const std::string& db_name,
                        const std::string& schema,
                        const std::string& db_table,
                        const std::string& srid = "3125",
                        const std::string& port = "5432",
                        const unsigned int& digits = 3,
                        const bool& binary = false,
                        const bool& drop_temporary_tables = true )
{
  Rcpp::Environment tmctools = Rcpp::Environment::namespace_env( "tmctools" );
  Rcpp::Function psql = tmctools["psql"];
  std::string connection_parameters = "postgresql://" + db_user + "@" + db_host + ":" + port + "/" + db_name + "?application_name=tmctools";
  std::string prefix = randomString( 5 );
  std::string sql;

  // Table container for unique points representing a vertex.
  sql =
    "CREATE TABLE " + prefix + "_point(" +
    "  id SERIAL PRIMARY KEY," +
    "  geom geometry(PointZ," + srid + ") NOT NULL UNIQUE" +
    ")";
  sendQuery( connection_parameters, sql );

  // Table container for the triangles. Each triangle will be composed of 3
  // rows with each row referencing to a  vertex.
  sql =
    "CREATE TABLE " + prefix + "_pointset(" +
    "  id SERIAL PRIMARY KEY," +
    "  tid integer NOT NULL REFERENCES " + schema + "." + db_table + ","
    "  pid integer NOT NULL," +
    "  geom geometry(PointZ," + srid + ") NOT NULL" +
    ")";
  sendQuery( connection_parameters, sql );

  sql =
    "INSERT INTO " + prefix + "_pointset (tid, pid, geom) " +
    "WITH cte_a AS (" +
    "  SELECT" +
    "    id," +
    "    (ST_DumpPoints(geom)).path[2] pid," +
    "    (ST_DumpPoints(geom)).geom geom " +
    "  FROM " + schema + "." + db_table +
    ") " +
    "SELECT id, pid, geom FROM cte_a WHERE pid <= 3";
  sendQuery( connection_parameters, sql );

  sql =
    "CREATE INDEX " + prefix + "_pointset_tpx " +
    "ON " + prefix + "_pointset (tid, pid) " +
    "WITH (fillfactor = 100)";
  sendQuery( connection_parameters, sql );

  sql =
    "CREATE INDEX " + prefix + "_pointset_gix " +
    "ON " + prefix + "_pointset USING gist (geom) " +
    "WITH (fillfactor = 100)";
  sendQuery( connection_parameters, sql );
  sql = "VACUUM ANALYZE " + prefix + "_pointset";
  psql( db_host, db_user, db_name, sql );

  sql =
    "INSERT INTO " + prefix + "_point (geom) " +
    "SELECT DISTINCT geom FROM " + prefix + "_pointset";
  sendQuery( connection_parameters, sql );

  sql =
    "CREATE INDEX " + prefix + "_point_gix " +
    "ON " + prefix + "_point USING gist (geom) " +
    "WITH (fillfactor = 100)";
  sendQuery( connection_parameters, sql );
  sql = "VACUUM ANALYZE " + prefix + "_point";
  psql( db_host, db_user, db_name, sql );

  if ( binary )
  {
    writePlyBinFromDB( prefix, connection_parameters );
  }
  else
  {
    writePlyHeaderFromDB( prefix + ".ply",
                          nrow( connection_parameters, "public", prefix + "_point" ),
                          nrow( connection_parameters, schema, db_table ) );
    writePlyVertexFromDB( connection_parameters, prefix, digits );
    writePlyFaceFromDB( connection_parameters, prefix );
  }

  if ( drop_temporary_tables )
  {
    sendQuery( connection_parameters, "DROP TABLE " + prefix + "_point" );
    sendQuery( connection_parameters, "DROP TABLE " + prefix + "_pointset" );
  }

  return prefix + ".ply";
}
