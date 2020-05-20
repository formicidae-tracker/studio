#<cldoc:ant_identification>

Ant Identification mechanism

# Ant Identification mechanism

The main object of interest in myrmidon are <fort::myrmidon::Ant>, designated by their
<fort::myrmidon::Ant::ID>. Their position is acquired by the FORT
system using fiducial marker, but the general philosophy of myrmidon
is to hide the direct <fort::myrmidon::TagID> value for this tracking
data, and asks user to relate <fort::myrmidon::TagID> to the wanted
<fort::myrmidon::Ant> object using
<fort::myrmidon::Identification>. The main reason is that with long
tracking experiment, some ant will need to be re-tagged, and one may
need to associate several <fort::myrmidon::TagID> to track the same
<fort::myrmidon::Ant>. Also in practice fiducial tag cannot be always
be aligned and centered on the insect body. Then
<fort::myrmidon::Identification> also contains data in order to make
these adjustment.

## Identifiying <fort::myrmidon::Ant> with <fort::myrmidon::Identification>

Usually this steps will be performed through the FORT Studio graphical
interface. However one would be able to add <fort::myrmidon::Ant> in
an <fort::myrmidon::Experiment> with
<fort::myrmidon::Experiment::CreateAnt> and then associate
<fort::myrmidon::TagID> with that <fort::myrmidon::Ant> using
<fort::myrmidon::Experiment::AddIdentification>.

<fort::myrmidon::Identification> have <fort::myrmidon::Time> validity,
using <fort::myrmidon::Identification::SetStart> and
<fort::myrmidon::Identification::SetEnd>, one can reduce the
<fort::myrmidon::Time> range an <fort::myrmidon::Identification> is
used to designate an <fort::myrmidon::Ant>

## Identification invariants

To ensure soundness of the tracking database, in an
<fort::myrmidon::Experiment>, myrmidon is ensuring that at a given
<fort::myrmidon::Time>:

 * Only one <fort::myrmidon::TagID> is used to designat an <fort::myrmidon::Ant>
 * Only one <fort::myrmidon::Ant> is targeted by any <fort::myrmidon::TagID>.


## Automatic and manual pose estimation

As mentionned above <fort::myrmidon::Identification> also contains data on how the
virtual <fort::myrmidon::Ant> reference frame is related to the tag reference frame.

Using the graphical interface, this orientation is automatically
computed when the user sets the head-tail measurement for any new
identification. However for very large number of <fort::myrmidon::Ant>, this is
unpraticable, and user can manually specify that relation using
<fort::myrmidon::Identification::SetUserDefinedAntPose>.
