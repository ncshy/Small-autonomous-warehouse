# Change to whatever your C++ compiler is
CC=g++

# Compile settings
CFLAGS=-g -c -Wall
LFLAGS=-lm -lpthread

# Source files
SOURCES=src/JSON.cpp src/JSONValue.cpp src/logger.cpp src/json_handler.cpp src/server.cpp
HEADERS=src/JSON.h src/JSONValue.h src/logger.h 
OBJECTS=$(SOURCES:src/%.cpp=obj/%.o)

# Output
EXECUTABLE=server

all:	$(SOURCES) $(EXECUTABLE)
	
$(EXECUTABLE):	$(OBJECTS) 
		$(CC) $(OBJECTS) -o $@ $(LFLAGS) 

obj/%.o:	src/%.cpp $(HEADERS)
		@test -d $(@D) || mkdir -p $(@D)
		$(CC) $(CFLAGS) $(@:obj/%.o=src/%.cpp) -o $@

clean:
		rm -f $(OBJECTS) $(EXECUTABLE)

