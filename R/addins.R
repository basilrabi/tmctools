insertCodeLink <- function() {
  rstudioapi::insertText("\\code{\\link{}}") # nocov
}

insertItem <- function() {
  rstudioapi::insertText("\\item{}{}")       # nocov
}
