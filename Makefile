CC = clang++ -std=c++17
CFLAGS = -g -Wall

TARGET = emu

all: $(TARGET)

$(TARGET): main.o chip8.o HandleInput.o
			$(CC) main.o chip8.o HandleInput.o -o $(TARGET)

main.o: main.cpp chip8.hpp HandleInput.hpp
			$(CC) $(CFLAGS) -c main.cpp

chip8.o: chip8.cpp chip8.hpp
			$(CC) $(CFLAGS) -c chip8.cpp

HandleInput.o: HandleInput.hpp HandleInput.cpp chip8.hpp
			$(CC) $(CFLAGS) -c HandleInput.cpp
			
clean:
	rm HandleInput.o chip8.o main.o $(TARGET)


