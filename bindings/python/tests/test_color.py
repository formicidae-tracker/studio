import py_fort_myrmidon as m
import unittest

class ColorTestCase(unittest.TestCase):

    def test_color_construction(self):
        c = m.Color(R = 0, G = 12, B = 34)
        ct = c.AsTuple()
        self.assertEqual(ct[0],0)
        self.assertEqual(ct[1],12)
        self.assertEqual(ct[2],34)
        with self.assertRaises(ValueError):
            c = m.Color((1,2,3,4))

        with self.assertRaises(ValueError):
            c = m.Color((1024,2,3))

        with self.assertRaises(ValueError):
            c = m.Color(R = 1024,G = 2,B = 3)

        with self.assertRaises(ValueError):
            c = m.Color(R = 1,G = 2048,B = 3)

        with self.assertRaises(ValueError):
            c = m.Color(R = 1,G = 2,B = -3)

    def test_color_format(self):
        c = m.Color(R = 0, G = 12, B = 34)
        self.assertEqual(c.__repr__(),"py_fort_myrmidon.Color(R = 0, G = 12, B = 34)")
        self.assertEqual(c.__str__(),"#000C22")
