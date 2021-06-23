import py_fort_myrmidon as m
import numpy.testing as npt
import unittest
import datetime

class ShapeTestCase(unittest.TestCase):
    def test_circle(self):
        c = m.Circle(center = [2,3],radius = 1)
        npt.assert_almost_equal(c.Center,[2,3])
        c.Center = [0,-1]
        npt.assert_almost_equal(c.Center,[0,-1])
        self.assertEqual(c.Radius,1)
        c.Radius = 2
        self.assertEqual(c.Radius,2)
        self.assertEqual(c.ShapeType,m.Shape.Type.CIRCLE)

    def test_capsule(self):
        c = m.Capsule(c1 = [1,2],c2 = [3,4],r1 = 5, r2 = 6)
        npt.assert_almost_equal(c.C1,[1,2])
        c.C1 = [0,0]
        npt.assert_almost_equal(c.C1,[0,0])
        self.assertEqual(c.R1,5)
        c.R1 = 6
        self.assertEqual(c.R1,6)
        npt.assert_almost_equal(c.C2,[3,4])
        c.C2 = [3,3]
        npt.assert_almost_equal(c.C2,[3,3])
        self.assertEqual(c.R2,6)
        c.R2 = 1
        self.assertEqual(c.R2,1)
        self.assertEqual(c.ShapeType,m.Shape.Type.CAPSULE)

    def test_polygon(self):
        p = m.Polygon(vertices = [[1,1],[1,-1],[-1,-1],[-1,1]])
        self.assertEqual(p.Size(),4)
        npt.assert_almost_equal(p.Vertex(0),[1,1])
        npt.assert_almost_equal(p.Vertex(1),[1,-1])
        npt.assert_almost_equal(p.Vertex(2),[-1,-1])
        npt.assert_almost_equal(p.Vertex(3),[-1,1])
        self.assertEqual(p.ShapeType,m.Shape.Type.POLYGON)
