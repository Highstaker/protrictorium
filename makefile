CC = g++
INCLUDES = 
LIBRARIES = -lglfw -lGL -lGLU -lpthread -lX11 -lXrandr
ADDITIONALS = 
SOURCE = $(wildcard *.cpp)
DEST = $(patsubst %.cpp,%,$(SOURCE))

all: 
	rm -f /mnt/ramtemp/$@
	 $(CC) main.cpp $(ADDITIONALS) -o /mnt/ramtemp/protrictorium $(LIBRARIES) $(INCLUDES)
	 
