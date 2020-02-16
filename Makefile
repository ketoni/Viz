
SOURCES = main.cpp win.cpp dynamicline.cpp math.cpp source.cpp recorder.cpp
EXECUTABLE = Viz


OBJECTS = $(patsubst %.cpp,%.o,$(SOURCES))
DEPENDS = $(OBJECTS:.o=.d)
CFLAGS =  -I . -c -g -MMD -MP -std=gnu++11 -Wall -pedantic
LFLAGS = -lsfml-audio -lsfml-graphics -lsfml-window -lsfml-system 
CC=clang++-3.8

all: $(SOURCES) $(EXECUTABLE)

$(EXECUTABLE): $(OBJECTS)
	$(CC) $(OBJECTS) $(LFLAGS) -o $@

.cpp.o:
	$(CC) $(CFLAGS) $< -o $@

.PHONY: clean
clean:
	rm -f $(EXECUTABLE) $(OBJECTS) $(DEPENDS)

run: $(EXECUTABLE)
	# rec -q -b 16 -r 44100 -t wav - | ./$(EXECUTABLE) 
	./$(EXECUTABLE) 

-include $(DEPENDS)
