#pragma once

#include <SDL.h>

bool init();
bool loadPixels(unsigned char* screen); 
Uint32 flipPixel(int row, int col);
