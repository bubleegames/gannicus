CC=g++
CFLAGS=-std=c++11 -g -O2 -DGL_GLEXT_PROTOTYPES -Wall
LINKER=-lGL -lSDLmain -lSDL_image -lSDL_mixer -lalut -lopenal -lGLU -lglut -lSDL
default: salt
all: bmpout salt 
salt: *.o src/game.cc
	$(CC) $(CFLAGS) $(LINKER) *.o src/game.cc -o salt
bmpout: *.o src/bmp.cc
	$(CC) $(CFLAGS) $(LINKER) *.o src/bmp.cc -o salt
gannicus.o: src/*/*.cc src/charlist.h
	$(CC) $(CFLAGS) $(LINKER) -c src/*/*.cc 
clean:
	rm -rf *.o
