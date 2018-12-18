# Change to whatever your C++ compiler is
CC=g++

# Compile settings
CFLAGS=-g -c -Wall
LFLAGS=-lm 

# Source files
SOURCES1=src/RX_only.cpp src/mrf24j.cpp src/spidev_transfer.cpp src/JSONValue.cpp src/JSON.cpp src/json_handler.cpp
SOURCES2=src/JSONValue.cpp src/logger.cpp src/JSON.cpp src/client.cpp
SOURCES3=src/JSONValue.cpp src/logger.cpp src/JSON.cpp src/zigbee_server.cpp src/json_handler.cpp
HEADERS1=src/mrf24j.h src/spidev_transfer.h src/JSONValue.h src/JSON.h src/json_handler.h
HEADERS2=src/DataBlock.h src/JSONValue.h src/logger.h src/JSON.h 
HEADERS3=src/DataBlock.h src/JSONValue.h src/logger.h src/JSON.h src/json_handler.h
OBJECTS1=$(SOURCES1:src/%.cpp=obj/%.o)
OBJECTS2=$(SOURCES2:src/%.cpp=obj/%.o)
OBJECTS3=$(SOURCES3:src/%.cpp=obj/%.o)

# Output
EXECUTABLE1=RX_only
EXECUTABLE2=udp_client
EXECUTABLE3=zigbee_server

all:	$(SOURCES1) $(SOURCES2) $(EXECUTABLE1) $(EXECUTABLE2) $(SOURCES3) $(EXECUTABLE3)
	
$(EXECUTABLE1):	$(OBJECTS1) 
		$(CC) $(OBJECTS1) -o $@ $(LFLAGS) 

$(EXECUTABLE2):	$(OBJECTS2) 
		$(CC) $(OBJECTS2) -o $@ $(LFLAGS) 

$(EXECUTABLE3):	$(OBJECTS3) 
		$(CC) $(OBJECTS3) -o $@ $(LFLAGS) 

obj/%.o:	src/%.cpp $(HEADERS)
		@test -d $(@D) || mkdir -p $(@D)
		$(CC) $(CFLAGS) $(@:obj/%.o=src/%.cpp) -o $@

clean:
		rm -f $(OBJECTS1) $(EXECUTABLE1) $(OBJECTS2) $(EXECUTABLE2) $(OBJECTS3) $(EXECUTABLE3)

