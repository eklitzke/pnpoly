#include <Python.h>

#include <stdio.h>


typedef float FLOAT_TYPE;

typedef struct {
    PyObject_HEAD
    size_t nvert;
    FLOAT_TYPE *vertx;
    FLOAT_TYPE *verty;
} pnpoly_object;



FLOAT_TYPE convert_to_float(PyObject *obj, int *err) {
    double val = PyFloat_AsDouble(obj);
    if (val == -1.0) {
        PyObject *err_obj = PyErr_Occurred();
        if (err_obj != NULL) {
            *err = 1;
        }
    }
    *err = 0;
    return (FLOAT_TYPE) val;
}


static void
pnpoly_dealloc(pnpoly_object *self) {
    free(self->vertx);
    free(self->verty);
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
    PyObject *vertices;
    if (!PyArg_ParseTuple(args, "O", &vertices)) {
        return -1;
    }
    if (!PyList_Check(vertices)) {
        return -1;
    }

    Py_ssize_t list_size = PyList_GET_SIZE(vertices);
    if (list_size < 3) {
        return -1;
    }

    self->nvert = (size_t) list_size;
    self->vertx = malloc(self->nvert * sizeof(FLOAT_TYPE));
    if (self->vertx == NULL) {
        return -1;
    }
    self->verty = malloc(self->nvert * sizeof(FLOAT_TYPE));
    if (self->verty == NULL) {
        free(self->vertx);
        return -1;
    }

    Py_ssize_t i;
    for (i = 0; i < self->nvert; i++) {
        PyObject *obj = PyList_GET_ITEM(vertices, i);
        if (!PyTuple_Check(obj)) {
            goto init_err;
        }
        if (PyTuple_Size(obj) != 2) {
            goto init_err;
        }

        PyObject *vertx = PyTuple_GET_ITEM(obj, 0);
        PyObject *verty = PyTuple_GET_ITEM(obj, 1);

        int err_occurred;
        self->vertx[i] = convert_to_float(vertx, &err_occurred);
        if (err_occurred) {
            goto init_err;
        }
        self->verty[i] = convert_to_float(verty, &err_occurred);
        if (err_occurred) {
            goto init_err;
        }
    }
    return 0;

 init_err:
    free(self->vertx);
    free(self->verty);
    return -1;
}

static PyObject *
pnpoly_contains(pnpoly_object *self, PyObject *args) {
    double testx, testy;
    if (!PyArg_ParseTuple(args, "dd", &testx, &testy)) {
        return -1;
    }
    int c = 0;
    size_t i, j;
    for (i = 0, j = self->nvert - 1; i < self->nvert; j = i++) {
        if (((self->verty[i] > testy) != (self->verty[j] > testy)) &&
            (testx < (self->vertx[j] - self->vertx[i]) *
             (testy - self->verty[i])
             / (self->verty[j] - self->verty[i]) + self->vertx[i]) ) {
            c = !c;
        }
    }
    if (c) {
        Py_RETURN_TRUE;
    }
    Py_RETURN_FALSE;
}

static PyMethodDef pnpoly_methods[] = {
    {"contains", (PyCFunction) pnpoly_contains, METH_VARARGS,
     "test if an x/y is in the polygon"},
    {NULL}
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
    0,                         /*tp_as_sequence*/
    0,                         /*tp_as_mapping*/
    0,                         /*tp_hash */
    0,                         /*tp_call*/
    0,                         /*tp_str*/
    0,                         /*tp_getattro*/
    0,                         /*tp_setattro*/
    0,                         /*tp_as_buffer*/
    Py_TPFLAGS_DEFAULT | Py_TPFLAGS_BASETYPE, /*tp_flags*/
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

    if (PyType_Ready(&pnpoly_type) < 0) {
        return;
    }

    m = Py_InitModule3("pnpoly", module_methods,
                       "Example module that creates an extension type.");

    if (m == NULL) {
      return;
    }

    Py_INCREF(&pnpoly_type);
    PyModule_AddObject(m, "Polygon", (PyObject *) &pnpoly_type);
}
