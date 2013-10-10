OSNAME := $(shell uname)

CCOMP= gcc

GL_H=
DEVICE_H=

ifeq ($(OSNAME), Darwin)

LIBPATH += -L"/System/Library/Frameworks/OpenGL.framework/Libraries"
FRAMEWORKS += -framework GLUT
FRAMEWORKS += -framework OpenGL
LIBS= $(LIBPATH) -lGL -lGLU -lm -lstdc++
LINKFLAGS=-g $(FRAMEWORKS)

else ifeq ($(OSNAME), Linux)

LIBS= -lGL -lGLU -lglut -lm -lstdc++
LINKFLAGS=-g

endif

CCFLAGS= -c -g -I./include


COMPATABILITYOBJS=

include Makefile.common

#-----------------------------------------------------------------------------#
# COMPATIBILITY ROUTINES                                                      #
#-----------------------------------------------------------------------------#

#objs/p-iris.o: src/p-iris.c include/math3d.h $(GL_H) $(DEVICE_H)
#	cc $(CCFLAGS) src/p-iris.c -o objs/p-iris.o

#objs/resetgrafx.o: src/resetgrafx.c $(GL_H) $(DEVICE_H)
#	cc $(CCFLAGS) src/resetgrafx.c -o obs/resetgrafx.o
