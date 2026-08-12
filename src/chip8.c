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

static bool chip8_fetch_opcode(const Chip8 *chip8, uint16_t *opcode)  {
    if (chip8->pc >= CHIP8_MEMORY_SIZE - 1) {
        return false;
    }

    uint8_t high = chip8->memory[chip8->pc];
    uint8_t low = chip8->memory[chip8->pc + 1];

    *opcode = (high << 8) | low;

    return true;
}

// DecodedOpcode
static DecodedOpcode chip8_decode(uint16_t opcode) {
    DecodedOpcode decoded = {
        .x = (opcode & 0x0F00) >> 8,
        .y = (opcode & 0x00F0) >> 4,
        .n = opcode & 0x00F,
        .nn = opcode & 0x00FF,
        .nnn = opcode & 0x0FFF
    };

    return decoded;
}
