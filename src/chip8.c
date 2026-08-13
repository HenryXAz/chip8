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

bool chip8_cycle(Chip8 *chip8) {
    if (chip8->halted) {
        return false;
    }

    uint16_t instruction_pc = chip8->pc;
    uint16_t opcode;

    if (!chip8_fetch_opcode(chip8, &opcode)) {
        fprintf(stderr, "Failed to fetch opcode at pc: 0x%03X\n", chip8->pc);

        chip8->halted = true;
        return false;
    }

    chip8->pc += 2; 

    if (!chip8_execute(chip8, opcode, instruction_pc)) {
        chip8->halted = true;
        return false; 
    }

    return true;
}

void chip8_dump_state(const Chip8 *chip8) {
    printf(
    "PC=0x%03X, I=0x%03X, SP=%u, "
    "VO=0x%0x2X, V1=0x%02X, V2=0x%02X, V3=0x%02X, "
    "VF=0x%02X\n",
    chip8->pc,
    chip8->I,
    chip8->sp,
    chip8->V[0],
    chip8->V[1],
    chip8->V[2],
    chip8->V[3],
    chip8->V[0xF]  
    ); 
}

static bool chip8_execute(
    Chip8 *chip8,
    uint16_t opcode,
    uint16_t instruction_pc
)
{
    DecodedOpcode decoded = chip8_decode(opcode);

    switch (opcode & 0xF000) {
        case 0x1000:
           chip8->pc = decoded.nnn;
           return true; 

        case 0x6000:
            chip8->V[decoded.x] = decoded.nn;
            return true;
        case 0x7000:
           chip8->V[decoded.x] += decoded.nn;
           return true; 
        case 0x8000:
           switch (decoded.n) {
                case 0x04: {
                    uint16_t result = (uint16_t) chip8->V[decoded.x] + chip8->V[decoded.y];
                    chip8->V[0xF] = (result > 0xFF) ? 1 : 0;
                    chip8->V[decoded.x] = (uint8_t) result;
                    return true;
                }
           }
        break;
    }

    fprintf(
        stderr,
        "Unknown opcode: 0x%04X at pc: 0x%03X\n",
        opcode,
        instruction_pc
    );

    return false;
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
