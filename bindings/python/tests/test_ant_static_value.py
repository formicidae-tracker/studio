import py_fort_myrmidon as m
import unittest


class AntStaticValue(unittest.TestCase):
    def test_value_binding(self):
        self.assertEqual(m.AntStaticValue.Type(False),"bool")
        self.assertEqual(m.AntStaticValue.Type(123),"int")
        self.assertEqual(m.AntStaticValue.Type(123.32),"float")
        self.assertEqual(m.AntStaticValue.Type("foo"),"string")
        self.assertEqual(m.AntStaticValue.Type(m.Time()),"py_fort_myrmidon.Time")
        with self.assertRaises(TypeError):
            m.AntStaticValue.Type(object());

        self.assertTrue(isinstance(m.AntStaticValue.TrueValue(),bool))
        self.assertTrue(isinstance(m.AntStaticValue.IntValue(),int))
        self.assertTrue(isinstance(m.AntStaticValue.FloatValue(),float))
        self.assertTrue(isinstance(m.AntStaticValue.StringValue(),str))
        self.assertTrue(isinstance(m.AntStaticValue.TimeValue(),m.Time))
