identMatrix <- matrix(c(1, 1, 1), nrow = 3)

#' Drape points to surface
#'
#' Gets the elevation of the points draped on a surface.
#'
#' @param collar 2D point simple feature with a 'geom' column
#' @param topo 3D polygon simple feature with a 'geom' column
#' @param digits number of decimal digits used for rounding of elevations
#' @return a vector containing the draped elevations of the points
#' @export drape
#' @importFrom dplyr left_join
#' @importFrom magrittr "%>%"
#' @importFrom sf st_coordinates st_crs st_geometry st_geometry_type
drape <- function(collar, topo, digits = 3) {
  if (sf::st_crs(collar) != sf::st_crs(topo))
    stop("collar and topo must have the same CRS")

  if (!all(sf::st_geometry_type(collar) == "POINT"))
    stop("collar must be all POINT geometry")

  if (!all(sf::st_geometry_type(topo) == "POLYGON"))
    stop("topo must be a POLYGON Z geometry")

  # Check if topo has 3D gesometry
  if (!"sfc" %in% class(topo$geom))
    stop("topo$geom is not a geometry column.")
  if (!"sfc" %in% class(collar$geom))
    stop("colar$geom is not a geometry column.")

  topo_geom_type <- as.data.frame.matrix(summary(topo))
  colnames(topo_geom_type) <- trimws(colnames(topo_geom_type))
  topo_geom_type <- as.character(topo_geom_type$geom)
  pattern <- "POLYGON Z\\s*:\\s*([0-9]+)\\s*"
  if (length(grep(pattern = pattern, topo_geom_type)) < 1)
    stop("topo must be a POLYGON Z geometry")

  # Check if all geometry in topo is 3D
  count_z <- sub(pattern, "\\1",
                 topo_geom_type[grepl(pattern, topo_geom_type)]) %>%
    as.integer()

  if (count_z < nrow(topo))
    stop("Not all geom in topo is a POLYGON Z geometry")

  # Create unique id
  if ("tid" %in% colnames(topo))
    warning("Overwriting 'tid' in topo.")
  topo$tid <- 1:nrow(topo)

  if ("cid" %in% colnames(collar))
    warning("Overwriting 'cid' in collar.")
  collar$cid <- 1:nrow(collar)

  # Get intersection
  suppressWarnings(overlap <- sf::st_intersection(collar[,"cid"], topo[,"tid"]))
  overlap <- overlap[, c("tid", "cid")]
  overlap$oid <- 1:nrow(overlap)
  sf::st_geometry(overlap) <- NULL

  # Remove duplicated cid due to point located at the edge of multiple polygons
  dupHoles <- overlap$oid[duplicated(overlap$cid)]
  overlap <- overlap[!overlap$oid %in% dupHoles, ]

  overlap <- dplyr::left_join(overlap, collar, by = c("cid"))
  overlap$X <- NA
  overlap$Y <- NA
  overlap[,c("X", "Y")] <- sf::st_coordinates(overlap$geom)
  overlap$geom <- NULL
  overlap$Z <- NA

  for (i in 1:nrow(overlap)) {
    tin <- topo$geom[overlap$tid[i]] %>% sf::st_coordinates()
    tin <- solve(tin[1:3, 1:3], identMatrix) %>% as.numeric()
    overlap$Z[i] <- (
      (1 - overlap$X[i] * tin[1] - overlap$Y[i] * tin[2] ) / tin[3]
    ) %>% round(digits = digits)
  }

  return(overlap$Z)
}


