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
    opcode = 0;

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
    for (int i = 0; i < size; ++i) {
        memory[i + 152] = buffer[i];
    }
    delete buffer;
    
    //initialize random seed, for use in opcodes.
    unsigned int t = (unsigned int) time(NULL);
    srand(t) ;
 
    return 0;
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
    
    opcode = (memory[pc] << 8) + (memory[pc + 1]); //Each opcode takes two entries of memory to be stored.
    
    //Switch statement to handle all opcodes one by one.
    switch(opcode & 0xF000) {
        case 0x0000: {
            switch(opcode & 0x0FFF) {
                case 0x00EE: {//0x00E0: Returns from a subroutine
                        pc = stack[--sp];
                        break;
                        }
                case 0x00E0: {//0x00E0: Clears the screen
                        std::fill(screen, screen + 32*64, 0);
                        draw = true;
                        pc += 2;
                        break;
                }
                default: {//case 0x0NNN;
                        break;
                }
            }
        }
        case 0x1000: {//0x1NNN: jumps to address NNN
            pc = opcode & 0x0FFF;
            break;
        }
        case 0x2000:{ //0x2NNN: jumps to subroutine at address NNN
            stack[sp++] = pc;
            pc = opcode & 0x0FFF;
            break;
        }
        case 0x3000: {//0x3XNN: Skips the next instruction if VX = NN
            pc += (registers[(opcode & 0x0F00) >> 8] == (opcode & 0x00FF)) ? 4 : 2;
            break;
        }
        case 0x4000: {//0x4XNN: Skips the next instruction if VX != NN
            pc += (registers[(opcode & 0x0F00) >> 8] != (opcode & 0x00FF)) ? 4 : 2;
            break;
        }
        case 0x5000: {//0x5XY0: Skips the next instruction if VX = VY
            pc += (registers[(opcode & 0x0F00) >> 8] == registers[opcode & 0x00F0]) ? 4 : 2;
            break;
        }
        case 0x6000: {//0x6XNN: Sets VX to NN
            registers[(opcode & 0x0F00) >> 8] = opcode & 0x00FF;
            pc += 2;
            break;
        }
        case 0x7000: {//0x7NNN: Adds NN to VX, no carry
            registers[(opcode & 0x0F00) >> 8] += opcode & 0x00FF;
            pc += 2;
            break;
        }
        
    case 0x8000:
        switch(opcode & 0x000F) {
        case 0x0000: //0x8XY0: Sets VX to VY
            registers[(opcode & 0x0F00) >> 8] = registers[(opcode & 0x00F0) >> 4];
            pc += 2;
            break;
        case 0x0001: //0x8XY1: Sets VX to VX | VY
            registers[(opcode & 0x0F00) >> 8] = registers[(opcode & 0x0F00) >> 8] | registers[(opcode & 0x00F0) >> 4];
            pc += 2;
        case 0x0002: //0x8XY2: Sets VX to VX & VY
            registers[(opcode & 0x0F00) >> 8] = registers[(opcode & 0x0F00) >> 8] & registers[(opcode & 0x00F0) >> 4];
            pc += 2;
            break;
        case 0x0003: //0x8XY3: Sets VX to VX xor VY
            registers[(opcode & 0x0F00) >> 8] = registers[(opcode & 0x0F00) >> 8] ^ registers[(opcode & 0x00F0) >> 4];
            pc += 2;
            break;
        case 0x0004: //0x8XY4: Adds VY to VX, with VF carry flag set if a carry occurs
            registers[15] = (registers[(opcode & 0x0F00) >> 8] > (0xFF - registers[(opcode & 0x00F0) >> 4])) ? 1 : 0;
            registers[(opcode & 0x0F00) >> 8] += registers[(opcode & 0x00F0) >> 4];
            pc += 2;
            break;
        case 0x0005: //0x8XY5: Subtract VY from VX, with VF carry flag set to 0 if borrow, else 1
            registers[15] = (registers[(opcode & 0x0F00) >> 8] < registers[(opcode & 0x00F0) >> 4]) ? 1 : 0;
            registers[(opcode & 0x0F00) >> 8] -= registers[(opcode & 0x00F0) >> 4];
            pc += 2;
            break;
        case 0x0006: //0x8XY6: Stores the least sig bit in VF, shifts VX left by 1
            registers[15] = registers[(opcode & 0x0F00) >> 8] % 10;
            registers[(opcode & 0x0F00) >> 8] = registers[(opcode & 0x0F00) >> 8] >> 1;
            pc += 2;
            break;
        case 0x0007: //0x8XY7: Sets VX to VY - VX.  Set VF to borrow flag.
            registers[15] = (registers[(opcode & 0x0F00) >> 8] > registers[(opcode & 0x00F0) >> 4]) ? 1 : 0;
            registers[(opcode & 0x0F00) >> 8] = registers[(opcode & 0x00F0) >> 4] = registers[(opcode & 0x0F00) >> 8];
            break;
        case 0x000E: //0x8XYE: Stores the most sig bit in VF, shifts VX right by 1
            registers[15] = (registers[(opcode & 0x0F00) >> 8] >> 15) % 10;
            registers[(opcode & 0x0F00) >> 8] = registers[(opcode & 0x0F00) >> 8] << 1;
            pc += 2;
            break;
        }
        
        case 0x9000: { //0x9NNN: Skips the next instruction if VX != VY
            pc += (registers[(opcode & 0x0F00) >> 8] == registers[(opcode & 0x00F0) >> 4]) ? 2 : 4;
            break;
        }
        case 0xA000: {//ANNN: Sets I to the address NNN
            I = opcode & 0x0FFF;
            pc += 2;
            break;
        }
        case 0xB000: {//0xBNNN: jumps to addres V0 + NNN
            pc = registers[0] + (opcode & 0x0FFF);
            break;
        }
        case 0xC000: {//0xCXNN: Sets VX to NN & (random number between 0 and 256)
            registers[(opcode & 0x0F00) >> 8] = (opcode & 0x00FF) & (rand() % 256);
            pc += 2;
            break;
        }
        case 0xD000: {//0xDXYN: Draws a sprite at (VX, VY) that is N+1 pixels high and 8 wide.
            int offset = 64*(opcode & 0x00F0 >> 4) + (opcode & 0x0F00 >> 8);
            if (offset > 32*64 - 1) return;
            int location = offset;
            unsigned short counter = I;
            for (int i = 0; i < (opcode & 0x000F) + 1; i++) {
                if ((screen[location] & memory[counter]) != (screen[location] ^ memory[counter])) {
                    registers[15] = 1;
                }
                for (int j = 0; j < 8; j++) {
                    screen[location + j] ^= (memory[counter] >> j) % 2;
                }
                location += 64;
                counter++;
            }
            draw = true;
            pc += 2;
            break;
        }
        case 0xE000: {
            switch(opcode & 0x00FF) {
                case 0x009E: {//0xEX93: Skips the next instruction if the key stored in VX is currently pressed
                    pc += (key_pressed && keyboard[registers[(opcode & 0x0F00) >> 8]] == 0) ? 2 : 4;
                    break;
                }
                case 0x00A1: {//0xEXA1: Skips the next instruction if the key stored in VY isn't currently pressed
                    pc += (!key_pressed || keyboard[registers[(opcode & 0x0F00) >> 8]] == 0) ? 4 : 2;
                    break;
                }
            }
        }
        
        case 0xF000: {
            switch(opcode & 0x00FF) {
                case 0x0007: {//0xFX07: Sets VX to the value of the delay timer
                    registers[(opcode & 0x0F00) >> 8] = delay_timer;
                    pc += 2;
                    break;
                }
                case 0x000A: {//0xFX0A: Awaits a key press, and stores key value in VX.
                    while (!key_event) ; // Wait for a key to be pressed.
                    memory[(opcode & 0x0F00) >> 8] = mapped_key;
                    pc += 2;
                    key_event = false;
                    break;
                }
                case 0x0015: {//0xFX15: Sets delay timer to VX
                    delay_timer = registers[(opcode & 0x0F00) >> 8];
                    break;
                }
                case 0x0018: {//0xFX18: Sets sound timer to VX
                    sound_timer = registers[(opcode & 0x0F00) >> 8];
                    break;
                }
                case 0x001E: {//0xFX1E: Adds VX to I
                    I += registers[(opcode & 0x0F00) >> 8];
                    break;
                }
                case 0x0029: {//0xFX29: Sets I to the location of the sprite in charcter VX (4x5 font).
                    I = registers[(opcode & 0x0F00) >> 8] * 5;
                    break;
                }
                case 0x0033: {//Store the binary-coded decimal rep of VX in memory[I:I+2]
                    memory[I] = registers[(opcode & 0x0F00) >> 8] / 100;
                    memory[I + 1] = (registers[(opcode & 0x0F00) >> 8] - memory[I]) / 10;
                    memory[I + 2] = (registers[(opcode & 0x0F00) >> 8] - memory[I] - memory[I + 1]);
                    pc += 2;
                    break;
                }
                case 0x0055: {//0xFX55: Stores V0 to VX in memory starting at address I
                    for (int i = 0; i < (((opcode & 0x0F00) >> 8) + 1); i++) {
                        memory[I + i] = registers[i];
                    }
                    I += (opcode & 0x0F00 >> 8) + 1; //Got this line from tutorial source code
                    break;
                }
                case 0x0065: { //0xFX65: Fills V0 to VX from memory starting at address I
                    for (int i = 0; i < (((opcode & 0x0F00) >> 8) + 1); i++) {
                        registers[i] = memory[I + i];
                    }
                    I += (opcode & 0x0F00 >> 8) + 1; //Got this line from tutorial source code
                    break;
                }
            }
        }
        
        default: {
            printf("Unknown opcode 0x%X\n", opcode);
        }
    }
}

