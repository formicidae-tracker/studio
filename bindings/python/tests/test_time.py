import py_fort_myrmidon as m
import unittest

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
