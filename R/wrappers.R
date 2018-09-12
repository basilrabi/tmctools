#' Wrapper for gstat::variogramLine
#'
#' Semivariance Values For a Given Variogram Model
#'
#' @param object variogram model for which we want semivariance function values
#' @param dist_vector numeric vector or matrix with distance values
#' @param ... other arguments passed to gstat::variogramLine
#' @return a data frame with distance and semi-variance columns or a distance
#'   matrix
#' @importFrom gstat variogramLine
variogramLine <- function(object, dist_vector, ...) {
  gstat::variogramLine(object = object, dist_vector = dist_vector, ...)
}
