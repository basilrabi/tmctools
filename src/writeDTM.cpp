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
//' @param dtmFile file name
//' @param user PostGIS user
//' @param hostname PostGIS server host name or IP address
//' @param dbname database to write into
//' @param tableName table to write into
//' @param schema schema to write into
//' @param port port number to connect to at the server host, or socket file
//'   name extension for Unix-domain connections
//' @param srid spatial reference identifier
//' @return number of rows uploaded to the table
//' @export
// [[Rcpp::export]]
int writeDTM( std::string dtmFile,
              std::string user,
              std::string hostname,
              std::string dbname,
              std::string tableName,
              std::string schema = "staging",
              std::string port = "5432",
              std::string srid = "3125" )
{
  std::string connectionParameters = "postgresql://" +
    user + "@" + hostname + ":" + port + "/" + dbname + "?application_name=tmctools";
  std::string sql;
  std::vector<DirVector> points;
  std::vector<Triangle> triangles;
  std::vector<Triangle>::iterator tri;
  unsigned int out = 0;

  if ( !fileExists( dtmFile ) )
    Rcpp::stop( dtmFile + " does not exist!" );

  std::string stringFile = dtmFile.substr( 0, dtmFile.size() - 3 ) + "str";
  points = readSTR( stringFile );
  triangles = pointToTri( points, dtmFile );

  pqxx::connection c{connectionParameters};
  pqxx::work txn{c};
  try
  {
    for ( tri = triangles.begin(); tri < triangles.end(); tri++ )
    {
      sql = "INSERT INTO " +
        pqxx::to_string( schema ) +
        "." +
        pqxx::to_string( tableName ) +
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
