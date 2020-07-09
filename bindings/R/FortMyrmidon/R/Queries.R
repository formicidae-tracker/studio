options("digits.secs" = 6)

#' Computes ant position during the experiment
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
#' @return a list of two element a) \code{frames} : a
#'     \code{data.frame} summarizing the time and space of each frame,
#'     and b) \code{position} a list of data.frame with the position
#'     of each ant for that frame. Each element of this list
#'     correspond to the same row in \code{frames}
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
#'     \code{e$const} if you didn't opened the experiment in read only
#'     mode.
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
#' @return a list of 4 elements a) \code{frames}, b) \code{positions}
#'     the two as in \code{\link{fmQueryIdentifyFrames}}, c)
#'     \code{collisions} a \code{data.frame} summarizing collision in
#'     each \code{frames}, and d) \code{types} a list of \code{matrix}
#'     with two column, indicating in the first column, the capsule
#'     type of the first ant, interacting in the second column the
#'     capsule type interacting in the second ant.
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
#' @return a list of two elements: a) \code{summaryTrajectories} a
#'     \code{data.frame} summarizing the ant, start time, and space of
#'     each trajectories reported, and b) \code{trajectories} a list
#'     of \code{data.frame} with the data point for every
#'     trajectories. Each element of this list correspond to the same
#'     row in the summary \code{data.frame}
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
#' @param reportGlobalTrajectories enables gloabl trajectory report in
#'     the result. If enabled two line in the resulting list will be
#'     added, as described by the output of
#'     \code{\link{fmQueryComputeAntTrajectories}}.
#' @param reportLocalTajectories for each interaction, copy the
#'     trajectory of each ant during that interaction. They will be
#'     reported in the final list objects as \code{ant1trajectory} and
#'     \code{ant2Trajectory}. If this option is not chosen, the mean
#'     position of each ant will be added in the main
#'     \code{interaction} \code{data.frame}, and two additionnal list
#'     \code{zone1} and \code{zone2} will be added to the main
#'     list. For each line of interactions, they contains a vector of
#'     all zone the corresponding ant was during that interaction.
#' @return a list with at list an element called \code{interactions}
#'     \code{data.frame} summarising all interactions. Other elements
#'     depends on the choice of othere report options.
fmQueryComputeAntInteractions <- function (experiment,
                                           start = NULL,
                                           end = NULL,
                                           maximumGap = fmSecond(1),
                                           matcher = NULL,
                                           singleThreaded = FALSE,
                                           showProgress = FALSE,
                                           reportGlobalTrajectories = FALSE,
                                           reportLocalTrajectories = FALSE) {
    if ( is.null(matcher) ) {
        matcher = fmMatcherAny();
    }

    return(fmQueryComputeAntInteractionsC(experiment,
                                          fmTimeCPtrFromAnySEXP(start),
                                          fmTimeCPtrFromAnySEXP(end),
                                          maximumGap,
                                          matcher,
                                          reportGlobalTrajectories,
                                          reportLocalTrajectories,
                                          singleThreaded,
                                          showProgress))
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
