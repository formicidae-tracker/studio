#' a S4 class that describes ant position
#'
#' @field frameTime the time of the frame as a POSIXct
#' @field width the width of the image
#' @field height the height of the image
#' @field space the associated \code{\link{fmSpace$spaceId}} with the
#'     frame
#' @field data a data.frame with the ID,X,Y Angle and optionally the
#'     ZoneID of the Ant in this frame
setClass("fmIdentifiedFrame", representation(
                                  "frameTime" = "POSIXct",
                                  "width"     = "integer",
                                  "height"    = "integer",
                                  "space"     = "integer",
                                  "data"      = "data.frame"
                              ))

#' a S4 class that describes ant collision (instantaneous interaction)
#'
#' @description describes an instantaneous interaction between two
#'     ants. We always have \code{ant1} < \code{ant2} to ensure
#'     uniqueness of the (ant1,ant2) couple.
#'
#' @field ant1 the ID of the first ant
#' @field ant2 the ID of the second ant
#' @field zone the ID of the zone where the collision happens
#' @field types a 2 column matrix with the virtual shape ID of the ant
#'     interacting, first column refers to the first ant, and second
#'     column to the second ant.
setClass("fmCollision",
         representation(
             "ant1"  = "integer",
             "ant2"  = "integer",
             "zone"  = "integer",
             "types" = "matrix"
         )
         )

#' a S4 class that describes ant collisions at a time
#'
#'
#' @field frameTime the time of the frame as a POSIXct
#' @field space the associated \code{\link{fmSpace$spaceId}} with the
#'     frame
#' @field collisions a list of \code{\link{fmCollision}} of the collision
#'     happening in that space and time.
setClass("fmCollisionFrame",
         representation(
             "frameTime"  = "POSIXct",
             "space"      = "integer",
             "collisions" = "list"
         )
         )

#' A S4 class describing an Ant trajectory
#'
#' @field ant the id of the ant
#' @field space the space this trajectory is happening
#' @field start the start of the trajectory as a POSIXct
#' @field positions a data.frame with the time,X,Y, Angle and optionally the
#'     ZoneID of the Ant. Time are second offsets from start.
setClass("fmAntTrajectory",
         representation(
             "ant"       = "integer",
             "space"     = "integer",
             "start"     = "POSIXct",
             "positions" = "data.frame"
         )
         )

#' A S4 class describing an interaction between two ant
#'
#' @field ant1 the id of the first ant
#' @field ant2 the id of the second ant
#' @field start the start of the interaction as a POSIXct
#' @field end the end of the interaction as a POSIXct
#' @field types a 2 column matrix with the virtual shape ID of the ant
#'     interacting, first column refers to the first ant, and second
#'     column to the second ant.
#' @field ant1Trajectory the trajectory of the first ant during the
#'     interaction, optionally reported.
#' @field ant2Trajectory the trajectory of the second ant during the
#'     interaction, optionally reported.
setClass("fmAntInteraction",
         representation(
             "ant1"           = "integer",
             "ant2"           = "integer",
             "start"          = "POSIXct",
             "end"            = "POSIXct",
             "types"          = "matrix",
             "ant1Trajectory" = "fmAntTrajectory",
             "ant2Trajectory" = "fmAntTrajectory"
         )
         )

options("digits.secs" = 6)

#' Computes ant position during the experiment
#'
#' @param experiment the \code{\link{fmCExperiment}} to query for, use
#'     \code{e$const} if you didn't opened the experiment in read
#'     only mode.
#' @param start the starting time for the query. Data which were
#'     acquire before that time will not be reported. Could be NULL
#'     for -∞, a POSIXct a fmTime or fmTimeCPtr object.
#' @param end the starting time for the query. Data which were acquire
#'     after that time will not be reported. Could be NULL for +∞, a
#'     POSIXct a fmTime or fmTimeCPtr object/
#' @param computeZones reports the user defined zone for the
#'     ants. otherwise the data.frame will be smaller and the
#'     computation faster.
#' @param singleThreaded perform a single threaded query. Could be
#'     faster for small experiment in number of Ant as identification
#'     is mostly IO bounded and the overhead induced by
#'     multi-threading synchronization impacts performances.
#' @param showProgress display the progress of the computation on the
#'     standard error output. It may not be portable behavior on all
#'     OS.
#' @return a list of \linkS4class{fmIdentifiedFrame}, ordered in time.
fmQueryIdentifyFrames <- function (experiment,
                                   start = NULL,
                                   end = NULL,
                                   computeZones = FALSE,
                                   singleThreaded = FALSE,
                                   showProgress = FALSE) {

    return(fmQueryIdentifyFramesC(experiment,
                                  fmTimeCPtrFromAnySEXP(start),
                                  fmTimeCPtrFromAnySEXP(end),
                                  computeZones,
                                  singleThreaded,
                                  showProgress))
}

#' Computes ant position and collision during the experiment
#'
#' @description Computes ant position and collision during the
#'     experiment. unlike \code{\link{fmQueryIdentifyFrames}} zone
#'     reporting will always occurs (it is precomputing steps for
#'     collision detection).
#'
#' @param experiment the \code{\link{fmCExperiment}} to query for, use
#'     \code{e$const} if you didn't opened the experiment in read
#'     only mode.
#' @param start the starting time for the query. Data which were
#'     acquire before that time will not be reported. Could be NULL
#'     for -∞, a POSIXct a fmTime or fmTimeCPtr object.
#' @param end the starting time for the query. Data which were acquire
#'     after that time will not be reported. Could be NULL for +∞, a
#'     POSIXct a fmTime or fmTimeCPtr object/
#' @param singleThreaded perform a single threaded query. Could be
#'     faster for small experiment in number of Ant as identification
#'     is mostly IO bounded and the overhead induced by
#'     multi-threading synchronization impacts performances.
#' @param showProgress display the progress of the computation on the
#'     standard error output. It may not be portable behavior on all
#'     OS.
#' @return a list of two lists. \code{$positions} a list of
#'     \linkS4class{fmIdentifiedFrame}, and \code{$collisions} a list
#'     of \linkS4class{fmCollisionFrame}. Both list refers to the same
#'     time and are ordered in time.
fmQueryCollideFrames <- function (experiment,
                                  start = NULL,
                                  end = NULL,
                                  singleThreaded = FALSE,
                                  showProgress = FALSE) {

    return(fmQueryCollideFramesC(experiment,
                                 fmTimeCPtrFromAnySEXP(start),
                                 fmTimeCPtrFromAnySEXP(end),
                                 singleThreaded,
                                 showProgress))
}

#' Computes ant trajectories during the experiment
#'
#' @param experiment the \code{\link{fmCExperiment}} to query for, use
#'     \code{e$const} if you didn't opened the experiment in read
#'     only mode.
#' @param start the starting time for the query. Data which were
#'     acquire before that time will not be reported. Could be NULL
#'     for -∞, a POSIXct a fmTime or fmTimeCPtr object.
#' @param end the starting time for the query. Data which were acquire
#'     after that time will not be reported. Could be NULL for +∞, a
#'     POSIXct a fmTime or fmTimeCPtr object/
#' @param maximumGap the maximum gap in tracking before cutting the
#'     trajectory in two different object. Use an insanely large value
#'     ( such as \code{fmHour(24*365)} to disable cutting of
#'     trajectory). Trajectory will always be cut when an ant moves
#'     from a space to another.
#' @param matcher a \linkS4class{fmMatcher} to reduce the query to
#'     wanted criterion. You can use \code{NULL} or
#'     \code{\link{fmMatcherAny()}} to match anything. Matcher that
#'     requires two ant will match anything.
#' @param computeZones reports the user defined zone for the
#'     ants. otherwise the data.frame will be smaller and the
#'     computation faster.
#' @param singleThreaded perform a single threaded query. Could be
#'     faster for small experiment in number of Ant as identification
#'     is mostly IO bounded and the overhead induced by
#'     multi-threading synchronization impacts performances.
#' @param showProgress display the progress of the computation on the
#'     standard error output. It may not be portable behavior on all
#'     OS.
#' @return a list of \linkS4class{fmAntTrajectory}, ordered in ending
#'     time.
fmQueryComputeAntTrajectories <- function (experiment,
                                           start = NULL,
                                           end = NULL,
                                           maximumGap = fmSecond(1),
                                           matcher = NULL,
                                           computeZones = FALSE,
                                           singleThreaded = FALSE,
                                           showProgress = FALSE) {
    if ( is.null(matcher) ) {
        matcher = fmMatcherAny();
    }

    return(fmQueryComputeAntTrajectoriesC(experiment,
                                          fmTimeCPtrFromAnySEXP(start),
                                          fmTimeCPtrFromAnySEXP(end),
                                          maximumGap,
                                          matcher,
                                          computeZones,
                                          singleThreaded,
                                          showProgress))
}

#' Computes ant interactions during the experiment
#'
#' @description Computes ant interactions and trajectories during the
#'     experiment. unlike \code{\link{fmQueryComputeAntTrajectory}} zone
#'     reporting will always occurs (it is a precomputing step for
#'     collision detection and therefore interaction computation).
#'
#' @param experiment the \code{\link{fmCExperiment}} to query for, use
#'     \code{e$const} if you didn't opened the experiment in read only
#'     mode.
#' @param start the starting time for the query. Data which were
#'     acquire before that time will not be reported. Could be NULL
#'     for -∞, a POSIXct a fmTime or fmTimeCPtr object.
#' @param end the starting time for the query. Data which were acquire
#'     after that time will not be reported. Could be NULL for +∞, a
#'     POSIXct a fmTime or fmTimeCPtr object/
#' @param maximumGap the maximum gap in tracking before cutting the
#'     trajectory in two different object. Use an insanely large value
#'     ( such as \code{fmHour(24*365)} to disable cutting of
#'     trajectory). Trajectories and Interactions will always be cut
#'     when an ant moves from a space to another.
#' @param matcher a \linkS4class{fmMatcher} to reduce the query to
#'     wanted criterion. You can use \code{NULL} or
#'     \code{\link{fmMatcherAny()}} to match anything. Matcher that
#'     requires two ant will match conditionally only for the
#'     interaction detection, but will always match for interaction
#'     detection. Matcher that addresses a single ant will match
#'     interaction where any of the two involved ant matches.
#' @param singleThreaded perform a single threaded query. Could be
#'     faster for small experiment in number of Ant as identification
#'     is mostly IO bounded and the overhead induced by
#'     multi-threading synchronization impacts performances.
#' @param showProgress display the progress of the computation on the
#'     standard error output. It may not be portable behavior on all
#'     OS.
#' @param reportTrajectories enables trajectory report in the
#'     \linkS4class{fmAntInteraction} objects. Due to the memory model
#'     of R, just copying this data from C++ to R will have a huge
#'     impact on performance ( may double computation time).
#' @return a list of two list. \code{$trajectories} is a list of
#'     \linkS4class{fmAntTrajectory}. \code{$interactions} is a list
#'     of \linkS4class{fmAntInteraction}. both list are ordered in
#'     ending of the corresponding object. Since Trajectory objects
#'     may span longer in time than interactions, the two lists do not
#'     have matching element (unlike the result of
#'     \code{\link{fmCollideFrames}}).
fmQueryComputeAntInteractions <- function (experiment,
                                           start = NULL,
                                           end = NULL,
                                           maximumGap = fmSecond(1),
                                           matcher = NULL,
                                           singleThreaded = FALSE,
                                           showProgress = FALSE,
                                           reportTrajectories = FALSE) {
    if ( is.null(matcher) ) {
        matcher = fmMatcherAny();
    }

    return(fmQueryComputeAntInteractionsC(experiment,
                                          fmTimeCPtrFromAnySEXP(start),
                                          fmTimeCPtrFromAnySEXP(end),
                                          maximumGap,
                                          matcher,
                                          singleThreaded,
                                          showProgress,
                                          reportTrajectories))
}

#' @name fmQueryComputeMeasurementFor
#' @title Computes manual measurement for an Ant
#' @description access and computes the manual measurement made in the
#'     FORT Studio GUI. Length are reported in mm from the detected
#'     tag size and the tag size parameter. See
#'     \url{https://formicidae-tracker.github.io/studio/docs/latest/api/#manual_measurement/manual_measurement}
#' @param experiment the \code{\link{fmCExperiment}} to query for, use
#'     \code{e$const} if you didn't opened the experiment in read only
#'     mode.
#' @param antID the ant we are interested in
#' @param mTypeID the measurement type id we are interceded in
#' @return a data.frame with the date of the measurement and the
#'     length in mm.
NULL

#' @name fmQueryComputeTagStatistics
#' @title Computes tag tracking statistics for an experiment
#' @description Computes tracking statistics by tag for an
#'     experiment. It consists of:
#'     \itemize{
#'     \item the dates at which a tag was first and last seen.
#'     \item The number of times it was detected \item the number of
#'     time it was detected multiple time in a frame
#'     \item an histogram of tracking gap for this tag. This histogram
#'     is only valid for experiment with a single space, and will
#'     report erroneous values for multiple space experiment.
#'     }
#' @param experiment the \code{\link{fmCExperiment}} to query for, use
#'     \code{e$const} if you didn't opened the experiment in read only
#'     mode.
#' @return a data.frame with the statistics for each tag.
NULL
