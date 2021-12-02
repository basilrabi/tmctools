#include <Rcpp.h>
#include <pqxx/pqxx>
#include <vector>
#include "utils.h"

//' Write Surpac DTM file
//'
//' Reads a valid DTM file and writes it into PostGIS. It is assumed that the
//'   PostGIS user has its pgpass set-up.
//'
//' It is also assumed that a table with access to a sequence is set-up like
//'   the \code{sql} query below:
//'   \preformatted{
//' CREATE SEQUENCE sequence_name
//' INCREMENT 1
//' START 1
//' MINVALUE 1
//' MAXVALUE 9223372036854775807
//' CACHE 1;
//'
//' CREATE TABLE table_name
//' (
//'   id integer NOT NULL DEFAULT nextval('sequence_name'::regclass),
//'   edge_length double precision,
//'   geom geometry(PolygonZ,3125),
//'   CONSTRAINT table_name_pkey PRIMARY KEY (id)
//' )}
//'
//' @param dtm file name
//' @param db_user PostGIS user
//' @param db_host PostGIS server host name or IP address
//' @param db_name database to write into
//' @param db_table table to write into
//' @param schema schema to write into
//' @param port port number to connect to at the server host, or socket file
//'   name extension for Unix-domain connections
//' @param srid spatial reference identifier
//' @return number of rows uploaded to the table
//' @export
// [[Rcpp::export]]
int writeDTM( std::string dtm,
              std::string db_user,
              std::string db_host,
              std::string db_name,
              std::string db_table,
              std::string schema = "staging",
              std::string port = "5432",
              std::string srid = "3125" )
{
  std::string connection_parameters = "postgresql://" + db_user + "@" + db_host + ":" + port + "/" + db_name + "?application_name=tmctools";
  std::string sql;
  std::vector<DirVector> points;
  std::vector<Triangle> triangles;
  std::vector<Triangle>::iterator tri;
  unsigned int out = 0;

  if ( !fileExists( dtm ) )
    Rcpp::stop( dtm + " does not exist!" );

  std::string stringFile = dtm.substr( 0, dtm.size() - 3 ) + "str";
  points = readSTR( stringFile );
  triangles = pointToTri( points, dtm );

  pqxx::connection c{connection_parameters};
  pqxx::work txn{c};
  try
  {
    for ( tri = triangles.begin(); tri < triangles.end(); tri++ )
    {
      sql = "INSERT INTO " +
        pqxx::to_string( schema ) +
        "." +
        pqxx::to_string( db_table ) +
        " (edge_length, geom) VALUES(" +
        pqxx::to_string( tri->longestEdge() ) +
        ", ST_GeomFromEWKT('" +
        pqxx::to_string( tri->ewkt( srid ) ) +
        "'))";
      txn.exec( sql );
      out++;
    }
    txn.commit();
  }
  catch ( std::exception const &e )
  {
    Rcpp::stop( e.what() );
  }


  return out;
}
