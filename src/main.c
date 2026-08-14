#include <stdio.h>
#include "chip8.h"

int main() 
{
    Chip8 chip8;
    chip8_init(&chip8);    

    uint8_t program[] = { 
        // // Test 1. Save and restore registers
        // /* 0x200 */ 0x60, 0x12, // V0 = 0x12
        // /* 0x202 */ 0x61, 0x34, // V1 = 0x34
        // /* 0x204 */ 0x62, 0x56, // V2 = 0x56
        // /* 0x206 */ 0xA3, 0x00, // I = 0x300
        // /* 0x208 */ 0xF2, 0x55, // save V0..V2
        
        // /* 0x20A */ 0x60, 0x00,
        // /* 0x20C */ 0x61, 0x00,
        // /* 0x20E */ 0x62, 0x00,
        
        // /* 0x210 */ 0xF2, 0x65, // restore V0..V2
        // /* 0x212 */ 0x12, 0x12  // loop

        // Test 2. BCD
        // 0x63, 0x7B, // V3 = 0x7B = 123
        // 0xA3, 0x50, // I = 0x350
        // 0xF3, 0x33, // BCD V3
        // 0x12, 0x06  // loop

        // Test 3. FX1E
        // 0x62, 0x05, // V2 = 5
        // 0xA3, 0x00, // I = 0x300
        // 0xF2, 0x1E, // I += V2
        // 0x12, 0x06

        // Test 4. Out-of-bounds FX55
        0xAF, 0xFF, // I = 0xFFF
        0xF2, 0x55  // attempt to write V0,V1,V2
    };

    if (!chip8_load_program(&chip8, program, sizeof(program))) {
        fprintf(stderr, "Program is too large to fit in memory\n");
        return 1;
    }

    for (int i = 0; i < 20; i++) {
        chip8_dump_state(&chip8);

        if(!chip8_cycle(&chip8)) {
            break;
        }
    }

    chip8_dump_state(&chip8);
    chip8_memory_dump(&chip8, 0x350, 3);

    return 0;
}
