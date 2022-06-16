library(DBI)
library(RPostgres)
library(sf)
library(tmctools)

con <- DBI::dbConnect(RPostgres::Postgres(),
                      dbname = "rtest",
                      host = Sys.getenv("R_TEST_HOST"),
                      port = 5432,
                      user = "rtest")

tableName <- gsub(x = paste0("topo-", Sys.Date()),
                  pattern = "-",
                  replacement = "_")
sequenceName <- paste0(tableName, "_id_seq")

sql <- paste("DROP TABLE IF EXISTS ", tableName)
DBI::dbExecute(con, sql)
sql <- paste("DROP SEQUENCE IF EXISTS ", sequenceName)
DBI::dbExecute(con, sql)

sql <- paste(
  "CREATE SEQUENCE", sequenceName,
  "INCREMENT 1",
  "START 1",
  "MINVALUE 1",
  "MAXVALUE 9223372036854775807",
  "CACHE 1"
)
DBI::dbExecute(con, sql)
sql <- paste0(
  "CREATE TABLE ", tableName, "\n",
  "(\n",
  "  id integer NOT NULL DEFAULT nextval('", sequenceName, "'::regclass),\n",
  "  edge_length double precision,\n",
  "  geom geometry(PolygonZ,3125),\n",
  "  CONSTRAINT ", tableName, "pkey PRIMARY KEY (id)\n",
  ")"
)
DBI::dbExecute(con, sql)

dtm_fle <- system.file("testdata", "eng.dtm", package = "tmctools")
testDF <- writeDTM( dtm_fle, "rtest", Sys.getenv("R_TEST_HOST"), "rtest", tableName, "public")

writtenData <- sf::read_sf(con, tableName)


test_that("writeDTM works", {
  expect_equal(round(writtenData$edge_length, digits = 2), 381.39)
  expect_equal(
    sf::st_as_text(writtenData$geom),
    "POLYGON Z ((586904.9 1054364 19.778, 586638.6 1054112 24.311, 586641.9 1054088 24.919, 586904.9 1054364 19.778))"
  )
})

sql <- paste("DROP TABLE IF EXISTS ", tableName)
DBI::dbExecute(con, sql)
sql <- paste("DROP SEQUENCE IF EXISTS ", sequenceName)
DBI::dbExecute(con, sql)

rm(list = ls())
