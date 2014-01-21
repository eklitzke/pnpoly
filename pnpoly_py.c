#include <Python.h>

#include "./pnpoly.h"

typedef struct {
    PyObject_HEAD
    polygon_t polygon;
} pnpoly_object;

// convert a Python float to a C float/double, with error checking
float_t convert_to_float(PyObject *obj, int *err) {
    double val = PyFloat_AsDouble(obj);
    if (val == -1.0) {
        PyObject *err_obj = PyErr_Occurred();
        if (err_obj != NULL) {
            *err = 1;
        }
    }
    return (float_t) val;
}


static void
pnpoly_dealloc(pnpoly_object *self) {
    polygon_dealloc(&self->polygon);
    self->ob_type->tp_free((PyObject *) self);
}

static PyObject *
pnpoly_new(PyTypeObject *type, PyObject *args, PyObject *kwds)
{
    pnpoly_object *self;
    self = (pnpoly_object *) type->tp_alloc(type, 0);
    return (PyObject *)self;
}

static int
pnpoly_init(pnpoly_object *self, PyObject *args) {
    PyObject *vertices = NULL;
    PyObject *obj = NULL;
    PyObject *fast_vertices = NULL;
    PyObject *fast_obj = NULL;

    if (!PyArg_ParseTuple(args, "O", &vertices)) {
        return -1;
    }

    fast_vertices = PySequence_Fast(vertices, "vertices must be a list");
    if (fast_vertices == NULL) {
        return -1;
    }
    Py_ssize_t list_size = PySequence_Fast_GET_SIZE(fast_vertices);
    if (list_size < 3) {
        PyErr_SetString(PyExc_ValueError, "vertex list too short");
        return -1;
    }

    if (polygon_alloc(&self->polygon, (size_t) list_size)) {
        PyErr_SetString(PyExc_MemoryError, "failed to allocate polygon");
        return -1;
    }

    size_t i;
    for (i = 0; i < self->polygon.nvert; i++) {
        obj = PySequence_Fast_GET_ITEM(fast_vertices, i);
        fast_obj = PySequence_Fast(obj, "vertex was not a sequence");
        if (fast_obj == NULL) {
            goto init_err;
        }
        if (PySequence_Fast_GET_SIZE(obj) != 2) {
            PyErr_SetString(PyExc_ValueError, "invalid vertex");
            goto init_err;
        }

        PyObject *vertx = PySequence_Fast_GET_ITEM(fast_obj, 0);
        PyObject *verty = PySequence_Fast_GET_ITEM(fast_obj, 1);

        int err_occurred = 0;
        self->polygon.vertx[i] = convert_to_float(vertx, &err_occurred);
        if (err_occurred) {
            goto init_err;
        }
        self->polygon.verty[i] = convert_to_float(verty, &err_occurred);
        if (err_occurred) {
            goto init_err;
        }
        Py_DECREF(fast_obj);
        fast_obj = NULL;
    }
    Py_DECREF(fast_vertices);
    return 0;

 init_err:
    Py_XDECREF(fast_obj);
    Py_XDECREF(fast_vertices);
    return -1;
}

static int
pnpoly_contains(pnpoly_object *self, PyObject *other) {
    if (!PySequence_Check(other)) {
        return 0;
    }
    int err = 0;
    float_t testx = convert_to_float(PySequence_GetItem(other, 0), &err);
    if (err) {
        return 0;
    }
    float_t testy = convert_to_float(PySequence_GetItem(other, 1), &err);
    if (err) {
        return 0;
    }
    if (polygon_contains(&self->polygon, testx, testy)) {
        return 1;
    }
    return 0;
}

static PyMethodDef pnpoly_methods[] = {
    {NULL}
};

static PySequenceMethods pnpoly_sequence_methods = {
    .sq_contains = (objobjproc) pnpoly_contains
};

static PyTypeObject pnpoly_type = {
    PyObject_HEAD_INIT(NULL)
    0,                         /*ob_size*/
    "Polygon",                 /*tp_name*/
    sizeof(pnpoly_object),     /*tp_basicsize*/
    0,                         /*tp_itemsize*/
    (destructor)pnpoly_dealloc, /*tp_dealloc*/
    0,                         /*tp_print*/
    0,                         /*tp_getattr*/
    0,                         /*tp_setattr*/
    0,                         /*tp_compare*/
    0,                         /*tp_repr*/
    0,                         /*tp_as_number*/
    &pnpoly_sequence_methods,  /*tp_as_sequence*/
    0,                         /*tp_as_mapping*/
    0,                         /*tp_hash */
    0,                         /*tp_call*/
    0,                         /*tp_str*/
    0,                         /*tp_getattro*/
    0,                         /*tp_setattro*/
    0,                         /*tp_as_buffer*/
    Py_TPFLAGS_DEFAULT | Py_TPFLAGS_BASETYPE | Py_TPFLAGS_HAVE_SEQUENCE_IN,
    /*tp_flags*/
    "a polygon",               /* tp_doc */
    0,                         /* tp_traverse */
    0,                         /* tp_clear */
    0,                         /* tp_richcompare */
    0,                         /* tp_weaklistoffset */
    0,                         /* tp_iter */
    0,                         /* tp_iternext */
    pnpoly_methods,            /* tp_methods */
    0,                         /* tp_members */
    0,                         /* tp_getset */
    0,                         /* tp_base */
    0,                         /* tp_dict */
    0,                         /* tp_descr_get */
    0,                         /* tp_descr_set */
    0,                         /* tp_dictoffset */
    (initproc) pnpoly_init,    /* tp_init */
    0,
    pnpoly_new,
};

typedef struct {
    PyObject_HEAD
    multi_polygon_t polygons;
} multi_pnpoly_object;

static void
multi_pnpoly_dealloc(multi_pnpoly_object *self) {
    multi_polygon_dealloc(&self->polygons);
    self->ob_type->tp_free((PyObject *) self);
}

static PyObject *
multi_pnpoly_new(PyTypeObject *type, PyObject *args, PyObject *kwds)
{
    multi_pnpoly_object *self;
    self = (multi_pnpoly_object *) type->tp_alloc(type, 0);
    return (PyObject *)self;
}

static int
multi_pnpoly_init(multi_pnpoly_object *self, PyObject *args) {
    size_t i, j;
    PyObject *vertices = NULL;
    PyObject *vertex_list = NULL;
    PyObject *obj = NULL;
    PyObject *fast_vertices = NULL;
    PyObject *fast_vertex_list = NULL;
    PyObject *fast_obj = NULL;

    if (!PyArg_ParseTuple(args, "O", &vertices)) {
        return -1;
    }

    fast_vertices = PySequence_Fast(vertices, "vertices must be a list");
    if (fast_vertices == NULL) {
        return -1;
    }
    Py_ssize_t list_size = PySequence_Fast_GET_SIZE(fast_vertices);
    if (list_size < 0) {
        return -1;
    }
    if (multi_polygon_alloc(&self->polygons, (size_t) list_size)) {
        PyErr_SetString(PyExc_MemoryError, "failed to allocate multi-polygon");
        return -1;
    }

    for (i = 0; i < self->polygons.npoly; i++) {
        vertex_list = PySequence_Fast_GET_ITEM(fast_vertices, i);
        fast_vertex_list = PySequence_Fast(vertex_list,
                                           "vertex was not a sequence");
        if (fast_vertex_list == NULL) {
            goto init_err;
        }

        Py_ssize_t list_length = PySequence_Fast_GET_SIZE(fast_vertex_list);
        if (list_length < 0) {
            goto init_err;
        } else if (list_length < 3) {
            PyErr_SetString(PyExc_ValueError, "vertex list too short");
            return -1;
        }

        size_t list_size = (size_t) list_length;
        if (polygon_alloc(self->polygons.polygons[i], list_length)) {
            goto init_err;
        }

        for (j = 0; j < list_size; j++) {
            obj = PySequence_Fast_GET_ITEM(fast_vertex_list, j);
            fast_obj = PySequence_Fast(obj, "vertex was not a sequence");
            if (fast_obj == NULL) {
                goto init_err;
            }
            if (PySequence_Fast_GET_SIZE(obj) != 2) {
                PyErr_SetString(PyExc_ValueError, "invalid vertex");
                goto init_err;
            }

            PyObject *vertx = PySequence_Fast_GET_ITEM(fast_obj, 0);
            PyObject *verty = PySequence_Fast_GET_ITEM(fast_obj, 1);

            int err_occurred = 0;
            self->polygons.polygons[i]->vertx[j] = convert_to_float(vertx, &err_occurred);
            if (err_occurred) {
                goto init_err;
            }
            self->polygons.polygons[i]->verty[j] = convert_to_float(verty, &err_occurred);
            if (err_occurred) {
                goto init_err;
            }
            Py_DECREF(fast_obj);
            fast_obj = NULL;
        }

        Py_DECREF(fast_vertex_list);
        fast_vertex_list = NULL;
    }
    Py_DECREF(fast_vertices);
    return 0;

 init_err:
    Py_XDECREF(fast_obj);
    Py_XDECREF(fast_vertex_list);
    Py_XDECREF(fast_vertices);
    return -1;
}

static int
multi_pnpoly_contains(multi_pnpoly_object *self, PyObject *other) {
    if (!PySequence_Check(other)) {
        return 0;
    }
    int err = 0;
    float_t testx = convert_to_float(PySequence_GetItem(other, 0), &err);
    if (err) {
        return 0;
    }
    float_t testy = convert_to_float(PySequence_GetItem(other, 1), &err);
    if (err) {
        return 0;
    }
    if (multi_polygon_contains(&self->polygons, testx, testy)) {
        return 1;
    }
    return 0;
}


static PyMethodDef multi_pnpoly_methods[] = {
    {NULL}
};

static PySequenceMethods multi_pnpoly_sequence_methods = {
    .sq_contains = (objobjproc) multi_pnpoly_contains
};

static PyTypeObject multi_pnpoly_type = {
    PyObject_HEAD_INIT(NULL)
    0,                         /*ob_size*/
    "MultiPolygon",                 /*tp_name*/
    sizeof(multi_pnpoly_object),     /*tp_basicsize*/
    0,                         /*tp_itemsize*/
    (destructor)multi_pnpoly_dealloc, /*tp_dealloc*/
    0,                         /*tp_print*/
    0,                         /*tp_getattr*/
    0,                         /*tp_setattr*/
    0,                         /*tp_compare*/
    0,                         /*tp_repr*/
    0,                         /*tp_as_number*/
    &multi_pnpoly_sequence_methods,  /*tp_as_sequence*/
    0,                         /*tp_as_mapping*/
    0,                         /*tp_hash */
    0,                         /*tp_call*/
    0,                         /*tp_str*/
    0,                         /*tp_getattro*/
    0,                         /*tp_setattro*/
    0,                         /*tp_as_buffer*/
    Py_TPFLAGS_DEFAULT | Py_TPFLAGS_BASETYPE | Py_TPFLAGS_HAVE_SEQUENCE_IN,
    /*tp_flags*/
    "some polygons",               /* tp_doc */
    0,                         /* tp_traverse */
    0,                         /* tp_clear */
    0,                         /* tp_richcompare */
    0,                         /* tp_weaklistoffset */
    0,                         /* tp_iter */
    0,                         /* tp_iternext */
    multi_pnpoly_methods,            /* tp_methods */
    0,                         /* tp_members */
    0,                         /* tp_getset */
    0,                         /* tp_base */
    0,                         /* tp_dict */
    0,                         /* tp_descr_get */
    0,                         /* tp_descr_set */
    0,                         /* tp_dictoffset */
    (initproc) multi_pnpoly_init,    /* tp_init */
    0,
    multi_pnpoly_new,
};


static PyMethodDef module_methods[] = {
    {NULL}  /* Sentinel */
};

#ifndef PyMODINIT_FUNC  /* declarations for DLL import/export */
#define PyMODINIT_FUNC void
#endif

PyMODINIT_FUNC
initpnpoly(void)
{
    PyObject* m;

    if (PyType_Ready(&pnpoly_type) < 0 || PyType_Ready(&multi_pnpoly_type) < 0) {
        return;
    }

    m = Py_InitModule3("pnpoly", module_methods,
                       "Example module that creates an extension type.");

    if (m == NULL) {
      return;
    }

    Py_INCREF(&pnpoly_type);
    Py_INCREF(&multi_pnpoly_type);
    PyModule_AddObject(m, "Polygon", (PyObject *) &pnpoly_type);
    PyModule_AddObject(m, "MultiPolygon", (PyObject *) &multi_pnpoly_type);
}
