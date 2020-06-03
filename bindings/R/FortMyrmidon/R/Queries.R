setClass("fmIdentifiedFrame",
         representation(
             "frameTime" = "POSIXct",
             "width"     = "integer",
             "height"    = "integer",
             "space"     = "integer",
             "data"      = "data.frame"
         )
         )

setClass("fmCollision",
         representation(
             "ant1"  = "integer",
             "ant2"  = "integer",
             "zone"  = "integer",
             "types" = "matrix"
         )
         )

setClass("fmCollisionFrame",
         representation(
             "frameTime"  = "POSIXct",
             "space"      = "integer",
             "collisions" = "list"
         )
         )

setClass("fmAntTrajectory",
         representation(
             "ant"       = "integer",
             "space"     = "integer",
             "start"     = "POSIXct",
             "positions" = "data.frame"
         )
         )

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
