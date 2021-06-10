.. api_time:

Time
====

``fort-myrmidon`` uses its own classes to represent
:cpp:class:`fort::Time` and :cpp:class:`fort::Duration` object. These
have been developped to handle the following features, which are not
available with ``std::chrono``, and with python and R time representations:

* Microseconds precision (actually up to nanosecond).
* Robustness to wall clock reset, that could naturally happen when the
  system clock is reset by ``ntp`` on windows, or when a leap second
  occurs. It uses a mix of wall and monotonic clocks (issued by the
  camera/framegrabber). It support multiple monotonic clock sources.
* Supports for representing -∞ and +∞ time concepts.


Specificities for R and python bindings
---------------------------------------

Due to python and R limitation, we cannot implictly convert native
time object of these object from and to :cpp:class:`fort::Time` and
:cpp:class:`fort::Duration`. Indeed:

* python ``time`` representation and R ``POSIXct`` as a ``double`` number of
  second from the epoch, is only guaranted 10us precision for time
  arround the year 2020. However the framegrabber used in the FORT
  issues time which are precise to the macrosecond. By converting
  :cpp:class:`fort::Time` objects to these native representation,
  rounding error may occurs and may change the behavior of certain
  query.
* R only uses 32-bit integer, that may overflow for duration of more
  than 2s.

Therefore the c++ classes :cpp:class:`fort::Time` and
:cpp:class:`fort::Duration` are binded to python and R. However the
two bindings differs a bit from the pure c++ implementation.

Python convertion to and from ``time.Time``
###########################################

``py_fort_myrmidon.Time`` objects can be created from and converted to a float
number of seconds from the epoch, as returned by ``time.time()`` or
``datetime.datetime.timestamp(self)``, with:

* ``py_fort_myrmidon.Time.__init__(self,t: float) -> py_fort_myrmidon.Time``

* ``py_fort_myrmidon.Time.ToTimestamp(self) -> float``

.. note::
   These conversions have a guaranted precision of only +/-10 us.

Python convertion to and from ``datetime.datetime``
###################################################

``py_fort_myrmidon.Time`` objects can be created from and converted to ``datetime.datetime``:

* ``py_fort_myrmidon.Time.__init__(self,t: datetime.datetime) -> py_fort_myrmidon.Time``
* ``py_fort_myrmidon.Time.ToDateTime(self) -> float``

However as cpython and c++17 lacks supports for timezone, these
``datetime.datetime`` objects are treated as naïve objects, i.e. object
wihtout an associated timezone, and assumed to be in local time. On
the contrary, :cpp:class:`fort::Time` uses only UTC time. It means
that:

* ``py_fort_myrmidon.Time.ToDateTime(self) -> float``, will return
  values that differs from one can read from the
  ``py_fort_myrmidon.Time.__str__(self)`` if not careful to take into
  account the local timezone.

* before passing any ``datetime.datetime`` object to
  ``py_fort_myrmidon.Time.__init__(self,dt: datetime.datetime)``, one
  must ensure they are converted to localtime first. One could use
  ``datetime.datetime.astimezone(self,None)`` to do that.

Python overloaded operators
###########################

One can use overloaded operators to naturally manipulate ``py_fort_myrmidon.Time`` objects.

.. code-block:: python

   import py_fort_myrmidon as fm

   start = fm.Time.Now()
   somefunction()
   ellapsed = fm.Time.Now() - start # equivalent to ellapsed = fm.Time.Now().Sub(start)

   later = start + 5 * fm.Duration.Second # equivalent to later = start.Add(5 * fm.Duration.Second)

   # we can compare Time and Duration
   later > start # equivalent to later.After(start)
   later < start # equivalent to later.Before(start)
   later == start # equivalent to later.Equals(start)
   later >= start
   later <= start

R conversion to and from ``POSIXct``
####################################

The ``fmTimeFromPOSIXct``/ ``fmTimeToPOSIXct`` converts to and from POSIXct objects.


R ``fmTime`` and ``fmDuration`` manipulation
############################################

To avoids ambiguities, some methods such as
:cpp:func:`fort::Time::After()` or :cpp:func:`fort::Time::Sub()` are
not binded to R. Instead one can use logical arithmetic to manipulate
date and time.

.. code-block:: R

   library(FortMyrmidon)

   start <- fmTimeNow()
   somefunction()
   ellapsed <- fmTimeNow() - start

   later <- start + fmSecond(5.0)

   # we can compare time objects
   later > start
   later < start
   later == start
   later <= start
   later >= start


c++ `fort::Duration` reference
------------------------------

.. doxygenclass:: fort::Duration
   :members:

c++ `fort::Time` reference
------------------------------
.. doxygenclass:: fort::Time
   :members:
