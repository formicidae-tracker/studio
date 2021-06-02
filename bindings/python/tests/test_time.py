import py_fort_myrmidon as m
import unittest
import datetime

class DurationTestCase(unittest.TestCase):
    def test_constructors(self):
        self.assertEqual(m.Duration(),0)
        self.assertEqual(m.Duration(421),421)

    def test_arithmetic(self):
        self.assertEqual(m.Duration.Second + 1,int(1e9)+1)
        self.assertEqual(1 + m.Duration.Second,int(1e9)+1)
        self.assertEqual(m.Duration.Hour + m.Duration.Second,int(3601*1e9))

        self.assertEqual(m.Duration.Second - 1,int(1e9)-1)
        self.assertEqual(1 - m.Duration.Second,1-int(1e9))
        self.assertEqual(m.Duration.Hour - m.Duration.Second,int(3599*1e9))


        self.assertEqual(m.Duration()*m.Duration.Hour,0)
        self.assertEqual(1*m.Duration.Hour,int(3600 * 1e9))
        self.assertEqual(m.Duration.Hour*2,int(2*3600 * 1e9))

        self.assertTrue(m.Duration() == 0)
        self.assertTrue(m.Duration(1) > 0)
        self.assertFalse(m.Duration(1) < 0)

        self.assertTrue(m.Duration(1) != 0)


    def test_formatting(self):
        self.assertEqual(m.Duration().__str__(),"0s")
        self.assertEqual(m.Duration.Hour.__str__(),"1h0m0s")
        self.assertEqual(m.Duration.Minute.__str__(),"1m0s")
        self.assertEqual(m.Duration.Second.__str__(),"1s")
        self.assertEqual((34*m.Duration.Millisecond).__str__(),"34ms")
        self.assertEqual((m.Duration.Second+100*m.Duration.Microsecond).__str__(),"1.0001s")

    def test_constant(self):
        with self.assertRaises(Exception):
            self.assertEqual(m.Duration.Hour,int(3600*1e9))
            m.Duration.Hour = 0
        with self.assertRaises(Exception):
            self.assertEqual(m.Duration.Minute,int(60*1e9))
            m.Duration.Minute = 0
        with self.assertRaises(Exception):
            self.assertEqual(m.Duration.Second,int(1e9))
            m.Duration.Second = 0
        with self.assertRaises(Exception):
            self.assertEqual(m.Duration.Millisecond,int(1e6))
            m.Duration.Millisecond = 0
        with self.assertRaises(Exception):
            self.assertEqual(m.Duration.Microsecond,int(1e3))
            m.Duration.Microsecond = 0

class TimeTestCase(unittest.TestCase):
    def test_constructor(self):
        self.assertEqual(m.Time(),m.Time(0.0))
        self.assertEqual(m.Time(),0.0)

    def test_has_infinite_support(self):
        self.assertEqual(m.Time.SinceEver().ToTimestamp(),float('-inf'))
        self.assertEqual(m.Time.Forever().ToTimestamp(),float('inf'))
        self.assertEqual(m.Time(float('-inf')),m.Time.SinceEver())
        self.assertEqual(m.Time(float('inf')),m.Time.Forever())

    def test_has_math_support(self):
        t = m.Time.Now().Round(m.Duration.Second)

        # we makes a deep copy of the time we use by passing it forth
        # and back to a float
        u = m.Time(t.ToDateTime())

        self.assertEqual(t.Add(1).Sub(t),1)
        self.assertEqual(t.Add(1*m.Duration.Second).Sub(t),m.Duration.Second)

        # we can use the verbose comparators Equals/After/Before or
        # the overloaded operators
        self.assertFalse(t > t)
        self.assertFalse(t.After(t))
        self.assertFalse(t < t)
        self.assertFalse(t.Before(t))
        self.assertTrue(t.Add(1) > t)
        self.assertTrue(t.Add(1).After(t))
        self.assertFalse(t > t.Add(1))
        self.assertFalse(t.After(t.Add(1)))
        self.assertTrue(t == t)
        self.assertTrue(t.Equals(t))

        ## all modification did not modify the original t
        self.assertEqual(t,u)

    def test_datetime_conversion(self):
        # create a datetime from UTC, and convert it to localtime
        d = datetime.datetime.fromisoformat("2019-11-02T23:12:13.000014+00:00").astimezone(None)
        t = m.Time(d)
        self.assertEqual(t,m.Time.Parse("2019-11-02T23:12:13.000014Z"))
        self.assertEqual(d,t.ToDateTime().astimezone(None))
