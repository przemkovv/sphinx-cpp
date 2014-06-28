
# TODO --- docs...
#TARGET_LIBS := lib1 lib2 lib3

# TODO --- docs...
TARGET_APPS := sphinx sphinx_tests

# TODO --- docs...
CPP_COMPILER := g++
C_COMPILER := gcc

# TODO --- docs...
CPP_COMPILER_FLAGS := -Wall -std=c++1y  -g 
C_COMPILER_FLAGS :=

# TODO --- docs...
LINKER_FLAGS :=

# TODO --- docs...
INCLUDE_DIRS := src
sphinx_INCLUDE_DIRS := 
sphinx_tests_INCLUDE_DIRS := tests

# TODO --- docs...
LIBRARY_DIRS :=
sphinx_LIBRARY_DIRS :=
sphinx_tests_LIBRARY_DIRS :=

# TODO --- docs...
LIBRARIES := PocoUtil PocoFoundation PocoNet
sphinx_LIBRARIES := 
sphinx_tests_LIBRARIES := 

# TODO --- docs...
SRC_DIR := src
sphinx_SRC_DIR := src
sphinx_tests_SRC_DIR := tests

BUILD_DIR := build
sphinx_BUILD_DIR := build
sphinx_tests_BUILD_DIR := build

OBJ_DIR := build/obj
sphinx_OBJ_DIR := build/obj/sphinx
sphinx_tests_OBJ_DIR := build/obj/tests

DEP_DIR := build/dep
sphinx_DEP_DIR := build/dep/sphinx
sphinx_tests_DEP_DIR := build/dep/tests

include tools/magic-makefile/Makefile.magic


test: sphinx_tests
	./sphinx_tests
