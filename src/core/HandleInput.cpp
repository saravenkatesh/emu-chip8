#include "HandleInput.hpp"

int Mapping (int key) {
    int key_index = 0;
    //Implement key --> mapped_key
    return key_index;
}

void KeyPressed (chip8& Chip8, int key) {
    Chip8.key_event = true;
    Chip8.key_pressed = true;
    Chip8.mapped_key = Mapping(key);
    Chip8.keyboard[Chip8.mapped_key] = 1;
}

void KeyReleased (chip8& Chip8, int key) {
    Chip8.key_event = false;
    Chip8.key_pressed = false;
    if (Chip8.mapped_key == Mapping(key)) Chip8.mapped_key = -1;
    Chip8.keyboard[Chip8.mapped_key] = 0;
}

//TO DO: Implement key handling
void HandleInput (chip8& Chip8) {
    int key = -1;
    bool pressed = false;
    bool released = false;
    // key = output of key_event
    if (pressed) KeyPressed(Chip8, key);
    if (released) KeyReleased(Chip8, key);
        
    if (key == QUIT) Chip8.quit = true;
}
