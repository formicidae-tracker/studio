#' @name fmAntStaticValue
#' @title a wrapper for \code{fort::myrmidon::AntStaticValue}
#' @description \code{fort::myrmidon::AntStaticValue} are using C++17
#'     a \code{std::variant}. This wrapper aims to emulate the
#'     functioanility of these objects. since we cannot mimic the use
#'     of \code{std::get<TYPE>()} function, these R object uses the
#'     following methods.
#'     \cr\cr
#'     To create an fmStaticValue, please use the following function
#'     to create an object of the appropriate type:
#'     \itemize{
#'     \item \code{\link{fmAntStaticBool}}
#'     \item \code{\link{fmAntStaticInteger}}
#'     \item \code{\link{fmAntStaticNumeric}}
#'     \item \code{\link{fmAntStaticString}}
#'     \item \code{\link{fmAntStaticTime}}
#'     }
#'     \cr\cr
#'     A \code{fmAntStaticValue} as an assigned type, declared by
#'     \code{\link{fmAntMetadataType}}. It is not possible to convert
#'     autonatically one type to another (you have to do it manually).
#'
#' @field toBool cast the object to a boolean value, fails if the type
#'     is not \code{BOOL}
#' @field toInt cast the object to an integer value, fails if the type
#'     is not \code{INTEGER}
#' @field toNumeric cast the object to a numeric value, fails if the
#'     type is not \code{DOUBLE}
#' @field toString cast the object to a character value, fails if the
#'     type is not \code{STRING}
#' @field toTime cast the object to a \code{\link{fmTime}} value,
#'     fails if the type is not \code{TIME}
NULL

#' @name fmAntStaticBool
#' @title creates a \code{\link{fmAntStaticValue}} from a boolean
#' @param v the boolean to represent
#' @return a \code{\link{fmAntStaticValue}} of type \code{BOOL}, see
#'     \code{\link{fmAntMetadataType}}
NULL

#' @name fmAntStaticInteger
#' @title creates a \code{\link{fmAntStaticValue}} from an integer
#' @param v the integer to represent
#' @return a \code{\link{fmAntStaticValue}} of type \code{INTEGER},
#'     see \code{\link{fmAntMetadataType}}
NULL

#' @name fmAntStaticNumeric
#' @title creates a \code{\link{fmAntStaticValue}} from a numerical
#'     value
#' @param v the numeric to represent
#' @return a \code{\link{fmAntStaticValue}} of type \code{DOUBLE}, see
#'     \code{\link{fmAntMetadataType}}
NULL

#' @name fmAntStaticString
#' @title creates a \code{\link{fmAntStaticValue}} from a character
#'     vector
#' @param v the character vector to represent
#' @return a \code{\link{fmAntStaticValue}} of type \code{STRING}, see
#'     \code{\link{fmAntMetadataType}}
NULL

#' @name fmAntStaticTime
#' @title creates a \code{\link{fmAntStaticValue}} from a
#'     \code{\link{fmTime}}
#' @param v the \code{\link{fmTime}} to represent
#' @return a \code{\link{fmAntStaticValue}} of type \code{TIME}, see
#'     \code{\link{fmAntMetadataType}}
NULL
