from distutils.core import setup, Extension

setup(name="pnpoly", version="1.0",
      ext_modules=[Extension("pnpoly", ["pnpoly.c"])])
