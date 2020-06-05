#' @name fmColor
#' @title Reference class for a RGB color
#' @description this is a wrapper for the C++ class
#'     \href{https://formicidae-tracker.github.io/studio/docs/latest/api/#fort::myrmidon/fort::myrmidon::Color}{fort::myrmidon::Color}. Since
#'     \code{std::tuple} does not exist in R, it defines their own
#'     custom methods.
#'     \cr\cr
#'     fmColor can be created with \code{\link{fmRGBColor}} or
#'     \code{\link{fmDefaultPaletteColor}}
#'
#' @field red the red component of the color as an integer between 0 and 255
#' @field green the green component of the color as an integer between 0 and 255
#' @field blue the blue component of the color as an integer between 0 and 255
NULL

#' @name fmRGBColor
#' @title Creates an \code{\link{fmColor}} from its RGB value
#' @param xx an integer vectorr of size 3 with the RGB value in this
#'     order
#' @return the wanted \code{\link{fmColor}}
#' @examples
#' white <- fmRGBColor(c(255,255,255))
#' pureBlue <- fmRGBColor(c(0,0,255))
NULL

#' @name fmDefaultPaletteColor
#' @title Creates an \code{\link{fmColor}} from myrmidon palette
#' @param i an integer to draw a color from
#' @description myrmidon defines a color-blind friendly palette of 7
#'     colors that can be accessed with this method. #i does not need
#'     to be between 0 and 6 color will cycle through the 7 available
#'     colors.
#' @return the wanted \code{\link{fmColor}}
#' @examples
#' # the two fmColor below will be distinguishable for most color-blind people
#' firstColor <- fmDefaultPaletteColor(0)
#' secondColor <- fmDefaultPaletteColor(1)
NULL
