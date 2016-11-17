
SOURCES = main.cpp win.cpp dynamicline.cpp math.cpp
EXECUTABLE = Viz


OBJECTS = $(patsubst %.cpp,%.o,$(SOURCES))
DEPENDS = $(OBJECTS:.o=.d)
CFLAGS =  -I . -c -g -MMD -MP -std=c++11 -Wall -pedantic
LFLAGS = -lsfml-audio -lsfml-graphics -lsfml-window -lsfml-system 
CC=clang++-3.5

all: $(SOURCES) $(EXECUTABLE)

$(EXECUTABLE): $(OBJECTS)
	$(CC) $(OBJECTS) $(LFLAGS) -o $@

.cpp.o:
	$(CC) $(CFLAGS) $< -o $@

.PHONY: clean
clean:
	rm -f $(EXECUTABLE) $(OBJECTS) $(DEPENDS)

run: $(EXECUTABLE)
	./$(EXECUTABLE) # wav here

-include $(DEPENDS)
