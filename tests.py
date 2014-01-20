import pnpoly
import unittest

class PnPolyTestCase(unittest.TestCase):

    def setUp(self):
        super(PnPolyTestCase, self).setUp()
        self.triangle_points = [(-1, 0), (0, 1), (1, 0)]


    def test_in_polygon(self):
        poly = pnpoly.Polygon(self.triangle_points)
        self.assert_((0, 0) in poly)
        self.assert_((0, 0.5) in poly)

    def test_no_in_polygon(self):
        poly = pnpoly.Polygon(self.triangle_points)
        self.assertEquals((1, 1) in poly, False)

    def test_short_point_list(self):
        self.assertRaises(ValueError, pnpoly.Polygon, [])

    def test_point_is_not_tuple(self):
        self.assertRaises(ValueError, pnpoly.Polygon, [[1], 1, 1])

    def test_invalid_point_type(self):
        self.assertRaises(TypeError, pnpoly.Polygon, [1, 1, 1])


if __name__ == '__main__':
    unittest.main()
