CC = g++
CPPFLAGS = -m32 -O2 -Wall -Werror -std=gnu++14 -lm
LDFLAGS = -s 
CFILES = main.cpp mygrep.cpp syntax.cpp lexic.cpp
HFILES = mygrep.h syntax.h lexic.h
OBJECTS = $(CFILES:.cpp=.o) 
TARGET = mygrep

all : bin/$(TARGET)

bin/mygrep : $(OBJECTS) 
	$(CC) $(LDFLAGS) $^ -o $@ 

include deps.make

deps.make : $(CFILES) $(HFILES) 
	g++ -MM $(CFILES) > deps.make 

clean :
	-rm -f bin/*
	-rm -f *.o 
