#pragma once
#include <iostream>

class chip8 {

    unsigned char *memory;
    unsigned char *stack;
    unsigned char *registers;
    unsigned short I;
    unsigned char delay_timer;
    unsigned char sound_timer;
    unsigned short pc;
    unsigned char sp;
    
    //Font table, copied from tutorial
    static constexpr unsigned char fontset[80] =
    {
      0xF0, 0x90, 0x90, 0x90, 0xF0, // 0
      0x20, 0x60, 0x20, 0x20, 0x70, // 1
      0xF0, 0x10, 0xF0, 0x80, 0xF0, // 2
      0xF0, 0x10, 0xF0, 0x10, 0xF0, // 3
      0x90, 0x90, 0xF0, 0x10, 0x10, // 4
      0xF0, 0x80, 0xF0, 0x10, 0xF0, // 5
      0xF0, 0x80, 0xF0, 0x90, 0xF0, // 6
      0xF0, 0x10, 0x20, 0x40, 0x40, // 7
      0xF0, 0x90, 0xF0, 0x90, 0xF0, // 8
      0xF0, 0x90, 0xF0, 0x10, 0xF0, // 9
      0xF0, 0x90, 0xF0, 0x90, 0x90, // A
      0xE0, 0x90, 0xE0, 0x90, 0xE0, // B
      0xF0, 0x80, 0x80, 0x80, 0xF0, // C
      0xE0, 0x90, 0x90, 0x90, 0xE0, // D
      0xF0, 0x80, 0xF0, 0x80, 0xF0, // E
      0xF0, 0x80, 0xF0, 0x80, 0x80  // F
    };
    
    struct Opcodes {
        using opFunction = std::function<void (unsigned short opcode)>;
        Opcodes(opFunction opfct) : _opfct(opfct) {};
        opFunction _opfct;
    };
    
    Opcodes f0{ [&](unsigned short opcode) {pc = stack[--sp];} };
    Opcodes f1{ [&](unsigned short opcode) {
        std::fill(screen, screen + 32*64, 0);
        draw = true;
        pc += 2;
    }};
    Opcodes f2{ [&](unsigned short opcode) {;} };
    Opcodes f3{ [&](unsigned short opcode) {pc = opcode & 0x0FFF;} };
    Opcodes f4{ [&](unsigned short opcode) {
        stack[sp++] = pc;
        pc = opcode & 0x0FFF;
    }};
    Opcodes f5{ [&](unsigned short opcode) {pc += (registers[(opcode & 0x0F00) >> 8] == (opcode & 0x00FF)) ? 4 : 2;} };
    Opcodes f6{ [&](unsigned short opcode) {pc += (registers[(opcode & 0x0F00) >> 8] != (opcode & 0x00FF)) ? 4 : 2;} };
    Opcodes f7{ [&](unsigned short opcode) {pc += (registers[(opcode & 0x0F00) >> 8] == registers[opcode & 0x00F0]) ? 4 : 2;} };
    Opcodes f8{ [&](unsigned short opcode) {
        registers[(opcode & 0x0F00) >> 8] = opcode & 0x00FF;
        pc += 2;
    }};
    Opcodes f9{ [&](unsigned short opcode) {
        registers[(opcode & 0x0F00) >> 8] += opcode & 0x00FF;
        pc += 2;
    }};
    Opcodes f10{ [&](unsigned short opcode) {
        registers[(opcode & 0x0F00) >> 8] = registers[(opcode & 0x00F0) >> 4];
        pc += 2;
    } };
    Opcodes f11{ [&](unsigned short opcode) {
        registers[(opcode & 0x0F00) >> 8] = registers[(opcode & 0x0F00) >> 8] | registers[(opcode & 0x00F0) >> 4];
        pc += 2;
    } };
    Opcodes f12{ [&](unsigned short opcode) {
        registers[(opcode & 0x0F00) >> 8] = registers[(opcode & 0x0F00) >> 8] & registers[(opcode & 0x00F0) >> 4];
        pc += 2;
    } };
    Opcodes f13{ [&](unsigned short opcode) {
        registers[(opcode & 0x0F00) >> 8] = registers[(opcode & 0x0F00) >> 8] ^ registers[(opcode & 0x00F0) >> 4];
        pc += 2;
    } };
    Opcodes f14{ [&](unsigned short opcode) {
        registers[15] = (registers[(opcode & 0x0F00) >> 8] > (0xFF - registers[(opcode & 0x00F0) >> 4])) ? 1 : 0;
        registers[(opcode & 0x0F00) >> 8] += registers[(opcode & 0x00F0) >> 4];
        pc += 2;
    } };
    Opcodes f15{ [&](unsigned short opcode) {
        registers[15] = (registers[(opcode & 0x0F00) >> 8] < registers[(opcode & 0x00F0) >> 4]) ? 1 : 0;
        registers[(opcode & 0x0F00) >> 8] -= registers[(opcode & 0x00F0) >> 4];
        pc += 2;
    } };
    Opcodes f16{ [&](unsigned short opcode) {
        registers[15] = registers[(opcode & 0x0F00) >> 8] % 10;
        registers[(opcode & 0x0F00) >> 8] = registers[(opcode & 0x0F00) >> 8] >> 1;
        pc += 2;
    } };
    Opcodes f17{ [&](unsigned short opcode) {
        registers[15] = (registers[(opcode & 0x0F00) >> 8] > registers[(opcode & 0x00F0) >> 4]) ? 1 : 0;
        registers[(opcode & 0x0F00) >> 8] = registers[(opcode & 0x00F0) >> 4] = registers[(opcode & 0x0F00) >> 8];
    } };
    Opcodes f18{ [&](unsigned short opcode) {
        registers[15] = (registers[(opcode & 0x0F00) >> 8] >> 15) % 10;
        registers[(opcode & 0x0F00) >> 8] = registers[(opcode & 0x0F00) >> 8] << 1;
        pc += 2;
    } };
    Opcodes f19{ [&](unsigned short opcode) {pc += (registers[(opcode & 0x0F00) >> 8] == registers[(opcode & 0x00F0) >> 4]) ? 2 : 4;} };
    Opcodes f20{ [&](unsigned short opcode) {
        I = opcode & 0x0FFF;
        pc += 2;
    } };
    Opcodes f21{ [&](unsigned short opcode) {pc = registers[0] + (opcode & 0x0FFF);} };
    Opcodes f22{ [&](unsigned short opcode) {
        registers[(opcode & 0x0F00) >> 8] = (opcode & 0x00FF) & (rand() % 256);
        pc += 2;
    } };
    Opcodes f23{ [&](unsigned short opcode) {
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
    } };
    Opcodes f24{ [&](unsigned short opcode) {pc += (key_pressed && keyboard[registers[(opcode & 0x0F00) >> 8]] == 0) ? 2 : 4;} };
    Opcodes f25{ [&](unsigned short opcode) {pc += (!key_pressed || keyboard[registers[(opcode & 0x0F00) >> 8]] == 0) ? 4 : 2;} };
    Opcodes f26{ [&](unsigned short opcode) {
        registers[(opcode & 0x0F00) >> 8] = delay_timer;
        pc += 2;
    } };
    Opcodes f27{ [&](unsigned short opcode) {
        while (!key_event) ; // Wait for a key to be pressed.
        memory[(opcode & 0x0F00) >> 8] = mapped_key;
        pc += 2;
        key_event = false;
    } };
    Opcodes f28{ [&](unsigned short opcode) {delay_timer = registers[(opcode & 0x0F00) >> 8];} };
    Opcodes f29{ [&](unsigned short opcode) {sound_timer = registers[(opcode & 0x0F00) >> 8];} };
    Opcodes f30{ [&](unsigned short opcode) {I += registers[(opcode & 0x0F00) >> 8];} };
    Opcodes f31{ [&](unsigned short opcode) {I = registers[(opcode & 0x0F00) >> 8] * 5;} };
    Opcodes f32{ [&](unsigned short opcode) {
        memory[I] = registers[(opcode & 0x0F00) >> 8] / 100;
        memory[I + 1] = (registers[(opcode & 0x0F00) >> 8] - memory[I]) / 10;
        memory[I + 2] = (registers[(opcode & 0x0F00) >> 8] - memory[I] - memory[I + 1]);
        pc += 2;
    } };
    Opcodes f33{ [&](unsigned short opcode) {
        for (int i = 0; i < (((opcode & 0x0F00) >> 8) + 1); i++) {
            memory[I + i] = registers[i];
        }
        I += (opcode & 0x0F00 >> 8) + 1; //Got this line from tutorial source code
    } };
    Opcodes f34{ [&](unsigned short opcode) {
        for (int i = 0; i < (((opcode & 0x0F00) >> 8) + 1); i++) {
            registers[i] = memory[I + i];
        }
        I += (opcode & 0x0F00 >> 8) + 1; //Got this line from tutorial source code
    } };
    
    Opcodes (*allOpcodes[35]) = {
        &f0, &f1, &f2, &f3, &f4,
        &f5, &f6, &f7, &f8, &f9,
        &f10, &f11, &f12, &f13, &f14,
        &f15, &f16, &f17, &f18, &f19,
        &f20, &f21, &f22, &f23, &f24,
        &f25, &f26, &f27, &f28, &f29,
        &f30, &f31, &f32, &f33, &f34,
    };
    
 public:

    chip8 () ;

    ~chip8 ();
    
    bool quit;

    unsigned char *screen;
    
    unsigned char *keyboard;
    
    bool draw;
    
    bool key_event;
    
    bool key_pressed;
    
    int mapped_key;
    
    bool sound_event;
    
    void Initialize () ;
    
    int LoadRom (std::string str) ;
    
    void TestLoad () ;

    void ProcessOpcode () ;
    
    void PrintRegisters() ;
    
    int HashOpcode(unsigned short opcode);

} ;