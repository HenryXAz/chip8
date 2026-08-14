#include <stdio.h>
#include "chip8.h"

int main() 
{
    Chip8 chip8;
    chip8_init(&chip8);    

    uint8_t program[] = { 
        // Test 1. Conditionl loop
        // 0x61, 0x00, // 0x200: V1 = 0
        // 0x71, 0x01, // 0x202: V1 += 1
        // 0x31, 0x03, // 0x204: skip next if V1 == 3
        // 0x12, 0x02, // 0x206: JP 0x202
        // 0x62, 0x0A, // 0x208: V2 = 10
        // 0x12, 0x0A  // 0x20A: loop

        // Test 2 RET AND CALL
        // /* 0x200 */ 0x61, 0x05, // V1 = 5
        // /* 0x202 */ 0x22, 0x10, // CALL 0x210
        // /* 0x204 */ 0x62, 0x0A, // V2 = 10
        // /* 0x206 */ 0x12, 0x06, // loop
        
        // /* 0x208 */ 0x00, 0x00,
        // /* 0x20A */ 0x00, 0x00,
        // /* 0x20C */ 0x00, 0x00,
        // /* 0x20E */ 0x00, 0x00,
        
        // /* 0x210 */ 0x63, 0x0F, // V3 = 15
        // /* 0x212 */ 0x00, 0xEE  // RET

        // Test 3. underflow
        // 0x00, 0xEE

        // Test 4. overflow
        0x22, 0x00
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

    return 0;
}
