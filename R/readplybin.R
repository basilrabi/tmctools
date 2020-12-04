#' Translate Binary PLY File
#'
#' Translates a binary PLY file to ASCII format.
#'
#' @param inPly name of ply file to be translated
#' @param outPly name of output ply file
#' @return NULL
#' @export
plyBinToText <- function(inPly, outPly= "text.ply") {
  if (!file.exists(inPly))
    stop(inPly, "does not exist!")
  if (file.exists(outPly)) {
    warning(outPly, "exists and will be renamed.")
    file.rename(outPly, paste0(outPly, "_old"))
  }
  system(paste("pdal translate -i", inPly,
               "-o pdal.ply --writers.ply.precision=3"))
  nPoints <- readPlyVTK(inPly)
  system(paste("tail -n", nPoints, "pdal.ply >> text.ply"))
  system("cat vtk.ply >> text.ply")
  file.rename("text.ply", outPly)
  file.remove("pdal.ply", "vtk.ply")
  return(invisible(NULL))
}

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
