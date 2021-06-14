.. api_typedef:

API generic types
=================



Simple type definitions
-----------------------


.. doxygentypedef:: fort::myrmidon::TagID

.. doxygentypedef:: fort::myrmidon::AntID

.. doxygentypedef:: fort::myrmidon::ZoneID

.. doxygentypedef:: fort::myrmidon::SpaceID

.. doxygentypedef:: fort::myrmidon::AntShapeTypeID

.. doxygentypedef:: fort::myrmidon::MeasurementTypeID

.. doxygenvariable:: fort::myrmidon::HEAD_TAIL_MEASUREMENT_TYPE


Complex type definitions
------------------------

.. doxygentypedef:: fort::myrmidon::Vector2dList

.. doxygenenum:: fort::myrmidon::AntMetaDataType

.. doxygentypedef:: fort::myrmidon::AntStaticValue

.. doxygentypedef:: fort::myrmidon::TypedCapsuleList

.. doxygentypedef:: fort::myrmidon::InteractionID

.. doxygentypedef:: fort::myrmidon::InteractionTypes

Structures
----------

.. note:: To simplify R data manipulation, there are no structure
   associated with this type. Indeed most
   :cpp:class:`fort::myrmidon::Query` returns list of these
   objects. In R these queries does not return a ``slist`` of ``S3
   objects`` as it would be quite unpratical. Instead, they returns
   several values, with at least one summary ``data.frame`` which
   refers to index in one or several ``slist`` of ``data.frame``.

General Information queries
+++++++++++++++++++++++++++

.. doxygenstruct:: fort::myrmidon::ComputedMeasurement
   :members:

.. doxygenstruct:: fort::myrmidon::TagStatistics
   :members:

.. doxygenstruct:: fort::myrmidon::ExperimentDataInfo
   :members:

.. doxygenstruct:: fort::myrmidon::SpaceDataInfo
   :members:

.. doxygenstruct:: fort::myrmidon::TrackingDataDirectoryInfo
   :members:

Snapshot Queries
++++++++++++++++

.. doxygenstruct:: fort::myrmidon::IdentifiedFrame
   :members:

.. doxygenstruct:: fort::myrmidon::Collision
   :members:

.. doxygenstruct:: fort::myrmidon::CollisionFrame
   :members:


Time-spanning Queries
+++++++++++++++++++++


.. doxygenstruct:: fort::myrmidon::AntTrajectory
   :members:


.. doxygenstruct:: fort::myrmidon::AntTrajectorySegment
   :members:


.. doxygenstruct:: fort::myrmidon::AntInteraction
   :members:
