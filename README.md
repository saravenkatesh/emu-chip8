# CHIP-8 Emulator

A CHIP-8 is a virtual machine from the '70s, primarily developed for game developing.  Its
architecture is the following (notes from a talk):

![](specs-1.tiff)
![](specs-2.tiff)
![](specs-3.tiff)

## Setup and play

Run `make` from the `src` directory to build the executable `emu`.  To load a chip-8 file, run `./emu [path/to/file]`.  For example, to play pong,

    ./emu games/pong.ch8

**Resources**

https://en.wikipedia.org/wiki/CHIP-8#Virtual_machine_description

https://multigesture.net/articles/how-to-write-an-emulator-chip-8-interpreter/

http://devernay.free.fr/hacks/chip8/C8TECH10.HTM

https://retrocomputing.stackexchange.com/questions/358/how-are-held-down-keys-handled-in-chip-8
