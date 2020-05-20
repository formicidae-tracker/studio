#<cldoc:manual_measurement>

Ant manual measurement

# Ant manual measurement

Using the FORT studio gui, one can make manual Ant measurement on high
resolution frames. One can defines several measurement type with the
GUI or using <fort::myrmidon::Experiment::CreateMeasurementType>, rename them with
<fort::myrmidon::Experiment::SetMeasurementTypeName> or delete them
with <fort::myrmidon::Experiment::DeleteMeasurementType>

Measurement made with the GUI can be accessed though
<fort::myrmidon::ComputedMeasurement> using
<fort::myrmidon::Query::ComputeMeasurementFor>. These computed
measurement returns a value in millimeters, estimated from the tag
image size, and the value of
<fort::myrmidon::Identification::TagSize>. One can defines at default
tag size for all identification at the experiment level using
<fort::myrmidon::Experiment::SetDefaultTagSize> and only redefine this
tag size for particular identification using
<fort::myrmidon::Identification::SetTagSize>. The latter should be
used for Queens per example that often uses a tag of a different size.

Tag size should use the normal tag size convention for the family
used. For example, ARTag tag defines their size from the inner black
border, and apriltag from the outer white border (if
applicable). These are the size written on the tag sheets.
