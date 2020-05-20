#<cldoc:queries>

Complex queries and matchers.

# Data queries and matcher

Finally, the myrmidon API provides an interface to perform complex
queries on the tracking data set with the metho defined in the
<fort::myrmidon::Query> class.

## List of query

One can :

* Access manual measurement from the myrmidon API
* Access individual position within a frame
* Compute <fort::myrmidon::AntTrajectory> for ants.
* Compute <fort::myrmidon::AntInteraction> between ants.

All available query are defined in the <fort::myrmidon::Query> class.


## Object returned.

Queries returns certains object, which aims to be easily translatable
to other programming languages.

* <fort::myrmidon::IdentifiedFrame> : a data frame with ant position
  for a given time, in a given <fort::myrmidon::Space>
* <fort::myrmidon::CollisionFrame> : a data frame with ant collision
  for a given time, in a given <fort::myrmidon::Space>
* <fort::myrmidon::AntTrajectory> : the timed trajectory for an Ant,
  in a given <fort::myrmidon::Space>. If the ant actually leaves the
  space, the trrajectory is cutted and two trajectories are reported
  instead. Timing returned by this object can be safely used for
  derivation and speed computation. Please note that the timesteps
  could and should be variable.
* <fort::myrmidon::AntInteraction> : a timed interaction between two
  ants, with the trajectory of the ant.

** TODO ** : a structure to report the Ant zone relatively to time is
   still needed

## Complex queries: using <fort::myrmidon::Matcher>

For some query, one can use <fort::myrmidon::Matcher> to build complex queries.

For example one can compute all trajectories for a single
<fort::myrmidon::Ant> with ID 42 using:

```
using namespace fort::myrmidon;
std::vector<AntTrajectory::ConstPtr> trajectories;
Query::ComputeTrajectories(experiment,
                           trajectories,
                           {}, // nullptr for start time == from -∞
                           {}, //nullptr for end time == until +∞
                           500 * Duration::Millisecond, // only allows gap of 500ms, otherwise report a new trajectory
                           Matcher::AntIDMatcher(42));
```


All available matcher, included And or Or combination matcher could be
found in <fort::myrmidon::Matcher> class.