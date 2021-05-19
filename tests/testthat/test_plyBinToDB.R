library(RPostgres)
library(tmctools)

db_host <- "localhost"
db_user <- "rtest"
db_name <- "rtest"
db_table <- "testply"

ply <- system.file("testdata", "tribin.ply", package = "tmctools")
plyBinToDB(inPly = ply, user = db_user, hostname = db_host, dbname = db_name, tableName = db_table)
plyBinToText(inPly = ply)
dbPly <- exportPly(user = db_user, hostname = db_host, dbname = db_name, schema = "public", tableName = db_table)

plyA <- readPly("text.ply")
plyB <- readPly(dbPly)

test_that("plyBinToDB() works", {
  expect_equal(all(plyA$polygon == plyB$polygon), TRUE)
})

file.remove("text.ply", dbPly)
psql(db_host, db_user, db_name, "DROP TABLE testply")
