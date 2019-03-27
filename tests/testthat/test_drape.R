library(sf)
library(tmctools)

gpkg <- system.file("testdata", "drape.gpkg", package = "tmctools")

collar <- sf::st_read(gpkg, "collar")
topo <- sf::st_read(gpkg, "topo")

draped <- tmctools::drape(collar, topo)

test_that("drape works", {
  expect_equal(draped, c(88.094, 84.237, 88.238,
                         83.108, 85.979, 78.714,
                         86.249, 86.546, 85.339))
})

rm(list = ls())
