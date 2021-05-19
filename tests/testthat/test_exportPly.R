library(RPostgres)
library(tmctools)
library(tools)

db_host <- "localhost"
db_user <- "rtest"
db_name <- "rtest"

con <- DBI::dbConnect(RPostgres::Postgres(),
                      dbname = db_name,
                      host = db_host,
                      port = 5432,
                      user = db_user)

ply <- system.file("testdata", "tritext.ply", package = "tmctools")
plyDF <- readPly(ply)
plyDF$id <- 1:nrow(plyDF)

RPostgres::dbWriteTable(con, "testply", plyDF, overwrite = TRUE)
psql(
  db_host, db_user, db_name,
  "ALTER TABLE testply ADD COLUMN geom geometry(PolygonZ,3125)"
)
psql(
  db_host, db_user, db_name,
  "UPDATE testply SET geom = ST_GeomFromText(polygon, 3125)"
)
psql(db_host, db_user, db_name, "ALTER TABLE testply ADD PRIMARY KEY (id)")

test <- tmctools::exportPly("rtest", "localhost", "rtest", "public", "testply")
psql(db_host, db_user, db_name, "DROP TABLE testply")
testDF <- readPly(test)
file.remove(test)

test_that("exportPly() works", {
  expect_equal(sort(plyDF$polygon), sort(testDF$polygon))
})
