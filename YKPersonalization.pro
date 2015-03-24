TEMPLATE = subdirs
CONFIG += ordered
SUBDIRS = \
  lib \
  src \
  tests
src.depends = lib
tests.depends = lib

QMAKE_CLEAN += src/Makefile tests/Makefile lib/Makefile
