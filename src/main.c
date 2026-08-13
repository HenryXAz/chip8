#include <stdio.h>
#include "chip8.h"

int main() 
{
    Chip8 chip8;
    chip8_init(&chip8);    

    uint8_t program[] = { 
        0x65, 0x0A,
        0x66, 0x05,
        0x85, 0x65,
        0x12, 0x06
    };

    if (!chip8_load_program(&chip8, program, sizeof(program))) {
        fprintf(stderr, "Program is too large to fit in memory\n");
        return 1;
    }

    for (int i = 0; i < 6; i++) {
        chip8_dump_state(&chip8);

        if(!chip8_cycle(&chip8)) {
            break;
        }
    }

    chip8_dump_state(&chip8);

    return 0;
}
