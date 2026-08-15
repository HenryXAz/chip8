#include <stdio.h>
#include "chip8.h"

int main() 
{
    Chip8 chip8;
    chip8_init(&chip8);    

    uint8_t program[] = {  
        0xF0, 0x29,  // I = font address for V0
        0x61, 0x05,  // V1 = 5
        0x62, 0x03,  // V2 = 3
        0xD1, 0x25,  // draw
        0x12, 0x0A   // loop
    };

    if (!chip8_load_program(&chip8, program, sizeof(program))) {
        fprintf(stderr, "Program is too large to fit in memory\n");
        return 1;
    }

    for (int i = 0; i < 100; i++) {
        chip8_dump_state(&chip8);

        if(!chip8_cycle(&chip8)) {
            break;
        }
    }

    chip8_dump_state(&chip8);
    chip8_memory_dump(&chip8, 0x350, 3);
    chip8_dump_display(&chip8);

    return 0;
}
