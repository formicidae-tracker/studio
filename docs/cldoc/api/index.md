#<cldoc:index>

FORmicidae Tracker myrmidon API documentation

# FORmicidae Tracker myrmidon API documentation

This is the C++ documentation of the myrmidon API to access and
post-process tracking data of the FORmicidae Tracker. The myrmidon API
provides an efficient interface to perform queries on large Ant colony
tracking datasets. Given a collection of tracking data acquired from
the [FORT](https://github.com/formicidae-tracker) system, and metadata
associated with the colony, it provides an efficient C/C++ interface
to perform queries on these dataset.

The API is meant to be used with the FORT Studio GUI, has it was
mainly designed to be used with user manual imput for the
<fort::myrmdion::Ant> poses.

The main entry point of the API is the <fort::myrmidon::Experiment> to
build a database of <fort::myrmidon::Ant> that can then be queried for
interactions or trajectories.

## Main documentation topics

  * <tag_identification>
  * <named_values>
  * <ant_collision_detection>
  * <manual_measurement>
  * <queries>
