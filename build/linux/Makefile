# $Id$
#
# This is a Makefile to build the static library out of Nova engine

CC=g++
CCFLAGS=-g -Wall
DEFINES=-DNOVA_LINUX32
INCLUDES=-I../../core/include/ -I../../util/common/include/ \
	-I../../adaptation/include/ -I../../adaptation/linux/include/

LIBS=

SRC=../../util/common/src/ColorCube.cpp \
	../../util/common/src/TexturedCube.cpp \
	../../util/common/src/Torus.cpp \
	../../util/common/src/RenderingUtils.cpp \
	../../util/common/src/TextureFactory.cpp \
	../../util/common/src/Normalizer.cpp \
	../../adaptation/linux/src/FixedOperations.cpp \
	../../adaptation/linux/src/novalogging.cpp \
	../../core/src/VectorMath.cpp \
	../../core/src/Display.cpp \
	../../core/src/Texture.cpp \
	../../core/src/Node.cpp \
	../../core/src/Shape.cpp \
	../../core/src/Lights.cpp \
	../../core/src/Frustum.cpp \
	../../core/src/Renderer.cpp \
	../../core/src/Camera.cpp 

OBJ=$(SRC:.cpp=.o)
OUT=libnova3d.a

.SUFFIXES: .cpp

.cpp.o:
	$(CC) $(DEFINES) $(INCLUDES) $(CCFLAGS) $(LIBS) -c $< -o $@

$(OUT): $(OBJ)
	ar rcs $(OUT) $(OBJ)

clean:
	rm -f $(OBJ) $(OUT) Makefile.bak *~
