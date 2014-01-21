import pnpoly
import unittest

class PolygonTestCase(unittest.TestCase):

    def setUp(self):
        super(PolygonTestCase, self).setUp()
        self.triangle_points = [(-1, 0), (0, 1), (1, 0)]

    def test_in_polygon(self):
        poly = pnpoly.Polygon(self.triangle_points)
        self.assert_((0, 0) in poly)
        self.assert_((0, 0.5) in poly)

    def test_not_in_polygon(self):
        poly = pnpoly.Polygon(self.triangle_points)
        self.assertEquals((1, 1) in poly, False)

    def test_short_point_list(self):
        self.assertRaises(ValueError, pnpoly.Polygon, [])

    def test_point_is_not_tuple(self):
        self.assertRaises(ValueError, pnpoly.Polygon, [[1], 1, 1])

    def test_invalid_point_type(self):
        self.assertRaises(TypeError, pnpoly.Polygon, [1, 1, 1])


class MultiPolygonTestCase(unittest.TestCase):

    def setUp(self):
        super(MultiPolygonTestCase, self).setUp()
        self.polygons = [
            [(-1, 0), (0, 1), (1, 0)],
            [(1, 1), (1, 2), (2, 2), (2, 1)]]

    def test_in_polygon(self):
        poly = pnpoly.MultiPolygon(self.polygons)
        self.assert_((0, 0) in poly)
        self.assert_((0, 0.5) in poly)
        self.assert_((1.5, 1.5) in poly)

    def test_not_in_polygon(self):
        poly = pnpoly.MultiPolygon(self.polygons)
        self.assertEquals((-10, -10) in poly, False)

    def test_empty_polygon_list(self):
        poly = pnpoly.MultiPolygon([])
        self.assert_((0, 0) not in poly)

    def test_short_point_list(self):
        self.assertRaises(ValueError, pnpoly.MultiPolygon, [[]])

    def test_point_is_not_tuple(self):
        self.assertRaises(ValueError, pnpoly.MultiPolygon, [[1], 1, 1])

    def test_invalid_point_type(self):
        self.assertRaises(TypeError, pnpoly.MultiPolygon, [[1, 1, 1]])


if __name__ == '__main__':
    unittest.main()
