#<cldoc:ant_interaction>

Ant Interaction (collision-detection)

# Ant Interaction (collision-detection)

Each Ant can have a virtual body that can later be used to find
"interaction", or more exactly detect when this two virtual body
collides.


## Ant virtual shapping

One would certainly use the FORT Studio interface to define and
visualize these vortual bodies, but the myrmidon API also provides an
access to these strcuture. Ant virtual parts consists only of a
collection of <fort::myrmidon::Capsule>, i.e., two circle with the
convex region between these circles.

Each of the capsule must be assigned to a virtual body part. These are
defined at the experiment level using
<fort::myrmidon::Experiment::CreateAntShapeType>. Each shape type is
assigned an <fort::myrmidon::AntShapeTypeID> and has a user defined
name. A type can be removed using
<fort::myrmidon::Experiment::DeleteAntShapeType>.

Once a type is defined, one can start to add capsule for any Ant with
that type using <fort::myrmidon::Ant::AddCapsule>

## Zoning

To help to prune geometrically the reported collisions, one can define
<fort::myrmidon::Zone>. Ant will collide which each other only if
theyu are in the same zone. These zone have time varying gemoetries,
that consist of <fort::myrmidon::Capsule>, <fort::myrmidon::Circle>
and <fort::myrmidon::Polygon>

## Accessing collision and interaction

Two level are defined for interaction in myrmidon. For a single given
time, we speak of <fort::myrmidon::Collision>, which just reports the
two <fort::myrmidon::Ant::ID> in the collision, and which part in each
ant collides.

An <fort::myrmidon::AntInteraction> structure is a more complex
structure, that reports a collision that last in time, alongside the
ant trajectories.

Both computation are rather complex, and oar object of <queries>
