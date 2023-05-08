# $Id$
#
# This is a Makefile to build the static library out of Nova engine

CC=g++
CCFLAGS=
DEFINES=-DNOVA_LINUX32
INCLUDES=-I../../../core/include/ -I../../../util/common/include/ \
	-I../../../adaptation/include/ -I../../../adaptation/linux/include/ \
	-I./include

LIBS=-lnova3d -lSDL_image -lSDL -lGL
LIBDIR=-L../../../build/linux

SRC=./src/main.cpp

OBJ=$(SRC:.cpp=.o)
OUT=novademo

.SUFFIXES: .cpp

.cpp.o:
	@echo Compiling..
	$(CC) $(DEFINES) $(INCLUDES) $(CCFLAGS) $(LIBS) -c $< -o $@

$(OUT): $(OBJ)
	@echo Linking..
	$(CC) $^ $(LIBDIR) $(LIBS) -o $@

clean:
	rm -f $(OBJ) $(OUT) Makefile.bak *~
