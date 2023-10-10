CC = clang++ -std=c++17
CFLAGS = -g -Wall
LIBS = -L SDL2/lib/lib  -l SDL2-2.0.0
SOURCES = \
		core/*.cpp \
		core/*.hpp \
		graphics/*.cpp \
		graphics/*.hpp
OBJECTS = $(SOURCES:.cpp=.o)
INCLUDES = -I SDL2/include/SDL2

TARGET = core/emu

all: $(TARGET)

$(TARGET): $(OBJECTS)
			$(CC) $(INCLUDES) $(CFLAGS) -o $@ $^ $(LIBS)

core/main.o: core/main.cpp core/chip8.hpp core/HandleInput.hpp
			$(CC) $(INCLUDES) $(CFLAGS) -c core/main.cpp

core/chip8.o: core/chip8.cpp core/chip8.hpp
			$(CC) $(INCLUDES) $(CFLAGS) -c core/chip8.cpp

core/HandleInput.o: core/HandleInput.hpp core/HandleInput.cpp core/chip8.hpp
			$(CC) $(INCLUDES) $(CFLAGS) -c core/HandleInput.cpp
			
graphics/setupScreen.o: graphics/setupScreen.cpp graphics/setupScreen.hpp
			$(CC) $(INCLUDES) $(CFLAGS) -c graphics/setupScreen.cpp

%.o: %.cpp
			$(CC) $(INCLUDES) $(CFLAGS) -c $(input)

clean:
	rm core/HandleInput.o core/chip8.o core/main.o $(TARGET)


