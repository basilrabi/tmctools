#' Execute psql Query
#'
#' Execute an PostgreSQL query using psql
#'
#' @param db_host PostGIS server host name or IP address
#' @param db_user PostGIS user
#' @param db_name database
#' @param query a single query to be executed
#' @param ignore.stdout see \link{system}
#' @return NULL if success
#' @export psql
psql <- function(db_host, db_user, db_name, query, ignore.stdout = TRUE) {
  cmd <- paste0("psql -h ", db_host,
                " -U ", db_user,
                " -d ", db_name,
                " -c \"", query, "\"")
  if (system(cmd, ignore.stdout = ignore.stdout) != 0)
    stop("PSQL command error.")

  return(invisible(NULL))
}
