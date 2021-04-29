#include <OpenGl/gl.h>
#include <OpenGl/glu.h>
#include <GLUT/glut.h>
#include "chip8.hpp"
#include "HandleInput.hpp"

int main(int argc, char **argv) {
    chip8 Chip8;
    
    //SetUpGraphics();
    //SetUpInput();
    //HandleSoundEvents();
    //DrawGraphics();
    //ExecutionTiming();

    Chip8.Initialize();
    
    //Load game corresponding to first argument.
    if (!Chip8.LoadRom(argv[1])) {
        std::cout << "Could not load game" << std::endl;
        return -1;
    }
    
    //Change to while(!quit) after implementing key event handling.
    while (!Chip8.quit) {
        
        //HandleInput (Chip8);
        
        Chip8.ProcessOpcode();
        
        if (Chip8.draw) {
            //DrawGraphics()
            Chip8.draw = false; //Reset the draw flag
        }
        
        if (Chip8.sound_event) {
            //Make beep
            Chip8.sound_event = false;
        }
    }

    return 0;
}
