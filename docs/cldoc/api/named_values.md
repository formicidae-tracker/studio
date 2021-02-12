#<cldoc:named_values>

Non-tracking timed data

# Named values: Non-tracking timed data for Ant

On top of the tracking data acquired with the tracking system
<fort::myrmidon::Ant> can also store timed key based values. Keys are
non-empty string, and values are <fort::myrmidon::AntStaticValue>,
i.e. an union of boolean, number string or <fort::myrmidon::Time>.

## Defining column and their type.

Non-tracking Ant data, should follow a strict structure. The possible
key, or column name must be defined with
<fort::myrmidon::Experiment::AddMetadataColumn>. The column names must
be unique.

Each column has an assigned <fort::myrmidon::AntMetadataType>
specifying which kind of value could be used, and a default
<fort::myrmidon::AntStaticValue>. If an <fort::myrmidon::Ant> doesn't
specify a value, the default value will be returned.

Column can later be manipulated using
<fort::myrmidon::Experiment::DeleteMetadataColumn>,
<fort::myrmidon::Experiment::RenameAntMetadataColumn> and
<fort::myrmidon::Experiment::SetAntMetadataColumnType>.
<fort::myrmidon::Experiment::AntMetadataColumns> can be used to get
current columns name, types and default values.

## Settings and accessing values

Once a column is set, one can use <fort::myrmidon::Ant::SetValue> to
set a value starting from a specified time. Values are timed, if no
value is set prior to a time point, the default value for the column
will be returned. One can use <fort::myrmidon::Time::SinceEver> to
specify new starting time value for an ant.

Accessing values is performed using
<fort::myrmidon::Ant::GetValue>. It takes a time argument, specifying
at which time we wnat that value for.
