#include "chip8.h"

#include <stdio.h>
#include <string.h>

void chip8_init(Chip8 *chip8) {
    memset(chip8, 0, sizeof(*chip8));

    chip8->pc = CHIP8_PROGRAM_START;
}

bool chip8_load_program(Chip8 *chip8, const uint8_t *program, size_t size) {
    size_t memory_available = CHIP8_MEMORY_SIZE - CHIP8_PROGRAM_START;

    if (size > memory_available) {
        return 0;
    }

    memcpy(
        &chip8->memory[CHIP8_PROGRAM_START],
        program,
        size
    );

    return true;
}
