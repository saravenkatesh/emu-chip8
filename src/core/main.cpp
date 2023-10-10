//#include <SDL.h>
#include "chip8.hpp"
#include "HandleInput.hpp"
//#include "/Users/saravenkatesh/Documents/CS/emu-chip8/emu-chip8/src/graphics/setupScreen.hpp"

int main(int argc, char **argv) {
    chip8 Chip8;
    bool quit = false;
//    SDL_Event e;
    
    //init();
    //SetUpInput();
    //HandleSoundEvents();
    //ExecutionTiming();

    Chip8.Initialize();
    
    //Load game corresponding to first argument.
    if (Chip8.LoadRom(argv[1]) == -1) {
        std::cout << "Could not load game" << std::endl;
        return -1;
    }

    Chip8.TestLoad(); //For testing
    
    //Change to while(!Chip8.quit) after implementing key event handling.
    for (int j=0; j < 5; j++) {
         
	for (int i=0; i < 10; i++) {
         //while ( SDL_PollEvent(&e) ) {

             //if ( e.type == SDL_QUIT ) {
             //    quit = true;
             //    break;
             //}
            
            Chip8.ProcessOpcode();
            
            Chip8.PrintRegisters() ; //For testing
            
            if (Chip8.draw) {
                //loadPixels(Chip8.screen);
                Chip8.draw = false; //Reset the draw flag
            }
            
            if (Chip8.sound_event) {
                //Make beep
                Chip8.sound_event = false;
            }
        }
    }

    return 0;
}
