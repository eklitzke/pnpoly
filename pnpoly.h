#ifndef PNPOLY_H_
#define PNPOLY_H_

#ifndef FLOAT_TYPE
#define FLOAT_TYPE float
#endif

#include <stddef.h>

typedef FLOAT_TYPE float_t;

typedef struct {
    size_t nvert;
    float_t *vertx;
    float_t *verty;
} polygon_t;

// allocate a polygon
int polygon_alloc(polygon_t *polygon, size_t nvert);

// deallocate a polygon
void polygon_dealloc(polygon_t *polygon);

// test if a polygon contains a test point
int polygon_contains(polygon_t *polygon, double testx, double testy);

typedef struct {
    size_t npoly;
    polygon_t **polygons;
} multi_polygon_t;

// allocate a multi-polygon; return 0 on success, 1 on failure
int multi_polygon_alloc(multi_polygon_t *polygons, size_t npoly);

// deallocate a multi-polygon
void multi_polygon_dealloc(multi_polygon_t *polygons);

int multi_polygon_contains(multi_polygon_t *polys, double testx, double testy);

#endif
