library(tmctools)

ply <- system.file("testdata", "tribin.ply", package = "tmctools")
newFile <- "t e s t.ply"
file.copy(ply, newFile)
testDF <- readPly(newFile)

test_that("readPlyBin works", {
  expect_equal(round(testDF$edge_length[1:3], digits = 3),
               c(2.652, 3.459, 2.694))
  expect_equal(
    testDF$polygon[1:3],
    c("POLYGON Z ((587070.360 1052866.292 319.057,587070.084 1052864.195 319.482,587068.275 1052866.063 318.959,587070.360 1052866.292 319.057))",
      "POLYGON Z ((587070.084 1052864.195 319.482,587070.360 1052866.292 319.057,587070.898 1052867.506 318.899,587070.084 1052864.195 319.482))",
      "POLYGON Z ((587054.683 1052862.375 317.396,587053.027 1052864.014 317.608,587053.804 1052864.897 317.748,587054.683 1052862.375 317.396))")
  )
})
file.remove(newFile)
rm(list = ls())
