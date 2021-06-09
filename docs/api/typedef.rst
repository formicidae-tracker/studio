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

.. doxygenenum:: fort::myrmidon::AntMetadataType

.. doxygentypedef:: fort::myrmidon::AntStaticValue

.. doxygentypedef:: fort::myrmidon::TypedCapsuleList

Structures
----------

.. note:: To simplify R data manipulation, there are no structure
   associated with this type. Indeed most
   :cpp:class:`fort::myrmidon::Query` returns list of these
   objects. In R these queries does not return a ``slist`` of ``S3
   objects`` as it would be quite unpratical. Instead, they returns
   several values, with at least one summary ``data.frame`` which
   refers to index in one or several ``slist`` of ``data.frame``.

.. doxygenstruct:: fort::myrmidon::ComputedMeasurement
   :members:
