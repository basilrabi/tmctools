#https://github.com/luckyrandom/cmaker/commit/b85813ac2b7aef69932eca8fbb4fa0ec225e0af0
Sys.setenv("R_TESTS" = "")

library(testthat)
library(tmctools)

test_check("tmctools")
