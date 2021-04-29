//
//  HandleInput.hpp
//  myChipBad
//
//  Created by Sara Venkatesh on 4/27/21.
//

#pragma once

#include "chip8.hpp"

constexpr int QUIT = 0; //TO DO: Initialize

int Mapping (int key) ;

void KeyPressed (chip8& Chip8, int key) ;

void KeyReleased (chip8& Chip8, int key) ;

void HandleInput (chip8& Chip8) ;
