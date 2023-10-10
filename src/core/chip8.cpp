#include <iostream>
#include <fstream>
#include <stdlib.h>
#include <time.h>
#include "chip8.hpp"

chip8::chip8 () {}

chip8::~chip8 () {
    delete[] memory;
    delete[] registers;
    delete[] stack;
    delete[] screen;
    delete[] keyboard;
}

void chip8::Initialize () {

    quit = false;
    
    memory = new unsigned char[4096](); //initialize memory array to 0
    stack = new unsigned char[16](); //initialize call stack array to 0
    registers = new unsigned char[16](); //initialize array of registers to 0
    I = 0; //address register;

    keyboard = new unsigned char[16](); //initialize keyboard array to 0
    screen = new unsigned char[32*64](); //initialize display to 0
    
    delay_timer = 0; //gaming delay timer
    sound_timer = 0; //sound effects timer
    pc = 0x200; //program counter
    sp = 0; //call stack pointer

    draw = false; //draw = true indicates something needs to be drawn.
    key_event = false; //key_event = true indicates a key has been pressed.
    key_pressed = false; //key_pressed = true indicates a key is currently being held down
    mapped_key = -1; //keyboard[mapped_key] corresponds to the key pressed, if mapped_key > -1
    sound_event = true; //sound_event = sound timer has finished counting down.

    //Load built-in font set.
    for (int i = 0; i < 72; i++) {
        memory[i + 80] = fontset[i];
    }
}

int chip8::LoadRom (std::string str) {
    std::streampos size;
    char* buffer; //Need type 'char' (instead of type 'unsigned char') for 'file.read'
    
    //Load game 'str' in binary, with file pointer at EOF
    std::ifstream file;
    file.open(str, std::ios::binary|std::ios::ate);
    if (!file) return -1;
    //Copy file contents to a buffer of chars.
    size = file.tellg(); //size of file
    buffer = new char[size];
    file.seekg(0, std::ios::beg); //Set file pointer to beginning.
    if (!file.read(buffer, size)) return -1;
    file.close();
    
    //Copy buffer contents to memory, starting at memory[152] to avoid overwriting fonts.
    for (int i = 0; i < size; i++) {
        memory[i + 512] = (unsigned char)buffer[i];
    }
    delete buffer;
    
    //initialize random seed, for use in opcodes.
    unsigned int t = (unsigned int) time(NULL);
    srand(t) ;
 
    return 0;
}

void chip8::TestLoad () {
    std::ofstream file;
    file.open("test.ch8", std::ios::binary|std::ios::out);
    for (int i = 512; i < 4096; i++) {
        file << memory[i];
    }
    file.close();
}

int chip8::HashOpcode (unsigned short opcode) {
    int big = (opcode & 0xF000) >> 12;
    int small = (opcode & 0x00F);
    int hash = -1;
    
    if (big == 0) {
        switch (small) {
            case 0: {
                hash = 1;
                break;
            }
            case 14: {
                hash = 2;
                break;
            }
            default: {
                hash = 3;
                break;
            }
        }
    }
    else if (1 <= big  && big < 8) {
        hash = big + 2;
    }
    else if (big == 8) {
        if (0 <= small && small < 7) hash = small + 10;
        else hash = 18;
    }
    else if (big == 9) hash = 19;
    else if (10 <= big && big < 14) hash = big + 10;
    else if (big == 14) {
        switch (small) {
            case 14: {
                hash = 24;
                break;
            }
            case 1: {
                hash = 25;
                break;
            }
        }
    }
    else if (big == 15) {
        switch (small) {
            case 7: {
                hash = 26;
                break;
            }
            case 10: {
                hash = 27;
                break;
            }
            case 5: {
                switch (opcode & 0x00F0) {
                    case 0x0010: {
                        hash = 28;
                        break;
                    }
                    case 0x0050: {
                        hash = 33;
                        break;
                    }
                    case 0x0060: {
                        hash = 34;
                        break;
                    }
                }
            }
            case 8: {
                hash = 29;
                break;
            }
            case 15: {
                hash = 30;
                break;
            }
            case 9: {
                hash = 31;
                break;
            }
            case 3: {
                hash = 32;
                break;
            }
        }
    }
    return hash;
}

void chip8::ProcessOpcode () {

    //If sound timer is counting down, deincrement.  If sound timer reaches zero, signal sound_event.
    if (sound_timer > 0) {
        if (--sound_timer == 0) sound_event = true;
    }

    //If delay_timer is counting down, deincrement and return from Cycle immediately.
    if (delay_timer > 0) {
        --delay_timer;
        return;
    }
    
    unsigned short opcode = (memory[pc] << 8) + (memory[pc + 1]); //Each opcode takes two entries of memory to be stored.
    
    int hash = HashOpcode(opcode);
    
    std::cout << hash << std::endl;
    
    if (hash == -1) return;
    
    allOpcodes[hash]->_opfct(opcode);
}

void chip8::PrintRegisters () {
    for (int i = 0; i < 16; i++) {
        std::cout << std::bitset<8>(registers[i]);
        std::cout << " | ";
    }
    std::cout << std::endl;
}

