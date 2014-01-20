from distutils.core import setup, Extension

setup(
    name="pnpoly",
    version="0.2",
    ext_modules=[Extension(
        "pnpoly", ["pnpoly.c", "pnpoly_py.c"]
    )]
)
