#include "./pnpoly.h"

#include <stdlib.h>

static inline int
inline_contains(polygon_t *polygon, double testx, double testy) {
    int c = 0;
    size_t i, j;
    for (i = 0, j = polygon->nvert - 1; i < polygon->nvert; j = i++) {
        if (((polygon->verty[i] > testy) != (polygon->verty[j] > testy)) &&
            (testx < (polygon->vertx[j] - polygon->vertx[i]) *
             (testy - polygon->verty[i])
             / (polygon->verty[j] - polygon->verty[i]) + polygon->vertx[i]) ) {
            c = !c;
        }
    }
    return c;
}

// allocate a polygon
int
polygon_alloc(polygon_t *polygon, size_t nvert) {
    polygon->nvert = nvert;
    polygon->vertx = malloc(sizeof(float_t) * nvert);
    if (polygon->vertx == NULL) {
        return 1;
    }
    polygon->verty = malloc(sizeof(float_t) * nvert);
    if (polygon->verty == NULL) {
        free(polygon->vertx);
        return 1;
    }
    return 0;
}

// deallocate a polygon
void
polygon_dealloc(polygon_t *polygon) {
    if (polygon->vertx != NULL) {
        free(polygon->vertx);
        polygon->vertx = NULL;
    }
    if (polygon->verty != NULL) {
        free(polygon->verty);
        polygon->verty = NULL;
    }
}

// test if a polygon contains a test point
int
polygon_contains(polygon_t *polygon, double testx, double testy) {
    return inline_contains(polygon, testx, testy);
}

// allocate a multi-polygon; return 0 on success, 1 on failure
int
multi_polygon_alloc(multi_polygon_t *polygons, size_t npoly) {
    polygons->npoly = npoly;
    polygons->polygons = malloc(sizeof(polygon_t *) * npoly);
    return polygons->polygons == NULL;

}

void
multi_polygon_dealloc(multi_polygon_t *polygons) {
    if (polygons->polygons != NULL) {
        free(polygons->polygons);
    }
    polygons->polygons = NULL;
}

int
multi_polygon_contains(multi_polygon_t *polygons, double testx, double testy) {
    size_t i;
    for (i = 0; i < polygons->npoly; i++) {
        if (inline_contains(polygons->polygons[i], testx, testy)) {
            return 1;
        }
    }
    return 0;
}
