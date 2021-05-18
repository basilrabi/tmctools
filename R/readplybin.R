#' Read Binary PLY File
#'
#' Reads the ply file created in surpac.
#'
#' @param plyFile file name
#' @return a data.frame with the following columns:
#'   \describe{
#'     \item{polygon}{polygon in ewkt or wkt format}
#'     \item{edge_length}{length of the longest edge}
#'     \item{slope_angle}{angle in radians between z-axis and the polygon normal}
#'   }
#' @export
readPlyBin <- function(plyFile) {
  plyBinToText(plyFile)
  df <- readPlyText("text.ply")
  file.remove("text.ply")
  return(df)
}
