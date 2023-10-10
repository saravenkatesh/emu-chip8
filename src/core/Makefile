CC = clang -std=c++17
CFLAGS = -g -Wall
LIBS = -L lib/lib -l SDL2-2.0.0
SOURCES = \
		*.cpp \
		*.hpp
OBJECTS = $(SOURCES:.cpp=.o)
INCLUDES = -I include/SDL2

TARGET = emu

all: $(TARGET)

$(TARGET): main.o chip8.o HandleInput.o
			$(CC) HandleInput.o chip8.o main.o -o $(TARGET)

main.o: main.cpp chip8.hpp HandleInput.hpp 
			$(CC) $(CFLAGS) -c main.cpp

chip8.o: chip8.cpp chip8.hpp
			$(CC) $(CFLAGS) -c chip8.cpp

HandleInput.o: HandleInput.hpp HandleInput.cpp chip8.hpp
			$(CC) $(CFLAGS) -c HandleInput.cpp

clean:
	rm HandleInput.o chip8.o main.o $(TARGET)


