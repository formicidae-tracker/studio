setClass("fmIdentifiedFrame",
         representation (
             "frameTime" = "POSIXct",
             "width"     = "integer",
             "height"    = "integer",
             "space"     = "integer" ,
             "data"      = "data.frame"
         )
         )

setClass("fmCollision",
         representation (
             "ant1" = "integer",
             "ant2" = "integer",
             "zone" = "integer",
             "interactionTypes" = "matrix"
         )
         )

setClass("fmCollisionFrame",
         representation (
             "frameTime" = "POSIXct",
             "space" = "integer",
             "collisions" = "list"
         )
         )

setClass("fmAntTrajectory",
         representation (
             "ant" = "integer",
             "space" = "integer",
             "start" = "POSIXct",
             "positions" = "data.frame"
         )
         )

setClass("fmAntInteraction",
         representation (
             "ant1" = "integer",
             "ant2"= "integer",
             "start" = "POSIXct",
             "end" = "POSIXct",
             "types" = "matrix",
             "ant1Trajectory" = "fmAntTrajectory",
             "ant2Trajectory" = "fmAntTrajectory"
         )
         )

options('digits.secs' = 6 )
