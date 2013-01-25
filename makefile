CC = g++
INCLUDES = 
LIBRARIES = -lglfw -lGL -lGLU -lpthread -lX11 -lXrandr
ADDITIONALS = 
SOURCE = $(wildcard *.cpp)
DEST = $(patsubst %.cpp,%,$(SOURCE))

all: $(DEST)

%: %.cpp
	rm -f /mnt/ramtemp/$@
	 $(CC) $< $(ADDITIONALS) -o /mnt/ramtemp/$@ $(LIBRARIES) $(INCLUDES)
	 
