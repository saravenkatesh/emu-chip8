#include "setupScreen.hpp"

const int MAGNIFICATION =  16;
const int SCREEN_WIDTH = 64*MAGNIFICATION;
const int SCREEN_HEIGHT = 32*MAGNIFICATION;

SDL_Window* gWindow = NULL;
SDL_Surface *gScreenSurface = NULL;

bool init() {
    bool success = true;
    
    if ( SDL_Init (SDL_INIT_VIDEO) < 0 ) {
        printf( "Screen initialization failed" );
        success = false;
    }

    else {
        gWindow = SDL_CreateWindow( "CHIP8 Emulator", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, SCREEN_WIDTH, SCREEN_HEIGHT, SDL_WINDOW_SHOWN  );
        gScreenSurface = SDL_GetWindowSurface(gWindow);
    }

    return success;
}

Uint32 flipPixel(int row, int col) {
    Uint32 *pix = (Uint32*) gScreenSurface->pixels;
    if (pix[MAGNIFICATION*row*SCREEN_HEIGHT + col*MAGNIFICATION] == 0) return 4294967295;
    else if (pix[MAGNIFICATION*row*SCREEN_HEIGHT + col*MAGNIFICATION] == 4294967295) return 0;
    else return -1;
}

//Read pixel values from a buffer.
bool loadPixels(unsigned char* screen) {
    bool success = true;
    int offset; 
    int mag = 0;
    Uint32 color;
    Uint32* buffer = (Uint32*) gScreenSurface->pixels;

    SDL_UnlockSurface(gScreenSurface);
    for (int row = 0; row < 32; ++row) {
        for (int col = 0; col < 64; ++col) {
            offset = row*64+ col;
            if (screen[offset]) {
                color = flipPixel(row, col);
                if (color < 0) {
                    printf("Non-monochrome pixel color detected.");
                    success = false;
                }
                for (int i = 0; i < MAGNIFICATION; ++i) {
                    for (int j = 0; j < MAGNIFICATION; ++j) {
                        offset = (MAGNIFICATION*row + i)*SCREEN_HEIGHT + col*MAGNIFICATION;
                        buffer[offset + j] = color;
                    }
                }
            }
        }
    }

    SDL_LockSurface(gScreenSurface);
    SDL_UpdateWindowSurface(gWindow);

    return success;
}