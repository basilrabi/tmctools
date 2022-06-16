library(RPostgres)
library(tmctools)
library(tools)

db_host <- Sys.getenv("R_TEST_HOST")
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

testA <- tmctools::exportPly("rtest", Sys.getenv("R_TEST_HOST"), "rtest", "public", "testply")
testB <- tmctools::exportPly("rtest", Sys.getenv("R_TEST_HOST"), "rtest", "public", "testply", binary = TRUE)
testDFA <- readPly(testA)
testDFB <- readPly(testB)
file.remove(testA, testB)

test_that("exportPly() works", {
  expect_equal(sort(plyDF$polygon), sort(testDFA$polygon))
  expect_equal(sort(plyDF$polygon), sort(testDFB$polygon))
})

psql(db_host, db_user, db_name, "DROP TABLE testply")
