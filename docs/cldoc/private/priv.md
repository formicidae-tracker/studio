#<cldoc:fort::myrmidon::priv>

Private Interface for Ant metadata manipulation

This namespace contains all code base for the creation, edition and
computation for Ant tracking metadata.

*This interface is not aimed to be binary stable at all, as user shoudl
not link directly against it. All symbols should not be considered
publically available, and binary compatibility could be broke between
patch version.*

The main entry point for this library is through an <Experiment>,
representing a collection of actual tracking experiment for a single
Ant colony. This <Experiment> holds a collection of relative pointers
to at least one <TrackingDataDirectory> containing the pristine
tracking data from an actual experiment.

The main object of study is an <Ant>. The collection of <Ant> in an
<Experiment> is managed by an <Identifier> who is responsible of their
creation, deletion and the management of their respective
<Identification>.
