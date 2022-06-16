library(DBI)
library(RPostgres)
library(tmctools)

db_host <- Sys.getenv("R_TEST_HOST")
db_user <- "rtest"
db_name <- "rtest"
db_table <- "testply"

ply <- system.file("testdata", "tribin.ply", package = "tmctools")
plyBinToDB(ply = ply,
           db_user = db_user,
           db_host = db_host,
           db_name = db_name,
           db_table = db_table)
plyBinToText(in_ply = ply)
dbPly <- exportPly(db_user = db_user,
                   db_host = db_host,
                   db_name = db_name,
                   schema = "public",
                   db_table = db_table)

plyA <- readPly("text.ply")
plyB <- readPly(dbPly)

# Testing area computation
con <- DBI::dbConnect(RPostgres::Postgres(),
                      dbname = db_name,
                      host = db_host,
                      port = 5432,
                      user = db_user)
areaDB <- DBI::dbGetQuery(con, "select st_area(geom) from testply")
areaPly <- readPly(ply, "3125")

test_that("plyBinToDB() works", {
  expect_equal(all(plyA$polygon == plyB$polygon), TRUE)
  expect_equal(all(areaDB$st_area == areaPly$area_2d), TRUE)
})

file.remove("text.ply", dbPly)
psql(db_host, db_user, db_name, "DROP TABLE testply")
