CC=g++
CFLAGS=-std=c++11 -g -O2 -DGL_GLEXT_PROTOTYPES -Wall
LINKER=-lGL -lSDL_image -lSDL_mixer -lopenal -lGLU -lSDL
default: salt
all: bmpout salt 
salt: gannicus characters src/game.cc
	$(CC) $(CFLAGS) $(LINKER) *.o src/game.cc -o salt
bmpout: gannicus characters src/bmp.cc
	$(CC) $(CFLAGS) $(LINKER) *.o src/bmp.cc -o bmpout
characters: src/characters/*.cc src/charlist.h
	$(CC) $(CFLAGS) $(LINKER) -c src/characters/*.cc 
gannicus: src/engine/*.cc src/charlist.h
	$(CC) $(CFLAGS) $(LINKER) -c src/engine/*.cc 
clean:
	rm -rf *.o
