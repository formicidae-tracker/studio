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

## examples

### query for a single frame

In order to query for a single frame, simply use the
<fort::myrmidon::Time> of the frame and <fort::myrmidon::Time> + 1 ns

```
using fort::myrmidon;
Time frameTime; // time of the wanted frame
std::vector<CollisionData> collisions; // after query, shoudl have a single element.
Query::CollideFrame(experiment,
                    collisions,
                    std::make_shared<Time>(frameTime),
                    std::make_shared<Time>(frameTime.Add(1)));
// 1ns is the minimal gap between two times, only a single frame
// should be reported. Maybe one of a different space if it happens
// that two frames have been allegedly taken in the same nanosecond
// for multispace experiment.
```

Note: querying Frame by Frame is not advised, as it will be very IO
inefficient. If you need a sequence of frames, it is better to ask for
all frames at once. Even if only a few frames are needed, it may be
more efficient to query for all of them and drop the many unwanted ones.


### Gets all the interactions with the nurse group

```
using fort::myrmidon;

experiment->AddMetadataColumn("group",AntMetadataType::String,"worker");

// Adds some ant to the "nurse" group
antA->SetValue("group",{},"nurse");
antB->SetValue("group",{},"nurse");

// query now all interactions with the nurse group
std::vector<AntTrajectory::ConstPtr> trajectories;
std::vector<AntInteraction::ConstPtr> interactions;
Query::ComputeAntInteractions(experiment,
                              tajectories,
                              interactions,
                              {},
                              {},
                              2 * Duration::Second,
                              Matcher::AntColumnMatcher("group","nurse"));
```