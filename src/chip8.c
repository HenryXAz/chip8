#include "chip8.h"

#include <stdio.h>
#include <string.h>


typedef struct {
    uint8_t x;
    uint8_t y;
    uint8_t n;
    uint8_t nn;
    uint16_t nnn;
} DecodedOpcode;


static DecodedOpcode chip8_decode(uint16_t opcode) ;
static bool chip8_fetch_opcode(const Chip8 *chip8, uint16_t *opcode);
static bool chip8_execute(
    Chip8 *chip8,
    uint16_t opcode,
    uint16_t instruction_pc
);
static bool chip8_memory_range_valid(uint16_t start, size_t length);

static DecodedOpcode chip8_decode(uint16_t opcode) {
    DecodedOpcode decoded = {
        .x =  (uint8_t)  ((opcode & 0x0F00) >> 8),
        .y =  (uint8_t)  ((opcode & 0x00F0) >> 4),
        .n =  (uint8_t)  (opcode & 0x00F),
        .nn = (uint8_t)  (opcode & 0x00FF),
        .nnn = (uint16_t)(opcode & 0x0FFF)
    };

    return decoded;
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

static bool chip8_execute(
    Chip8 *chip8,
    uint16_t opcode,
    uint16_t instruction_pc
)
{
    DecodedOpcode decoded = chip8_decode(opcode);

    if (opcode == 0x00EE) {
        if (chip8->sp == 0) {
            fprintf(
                stderr,
                "Stack underflow at pc=0x%03X",
                (uint16_t) instruction_pc
            );

            return false;
        }

        chip8->sp--;
        chip8->pc = chip8->stack[chip8->sp];

        return true;
    }

    switch (opcode & 0xF000) {
        case 0x1000:
           chip8->pc = decoded.nnn;
           return true; 
        case 0x2000: {
            if (chip8->sp >= CHIP8_STACK_SIZE) {
                fprintf(
                    stderr,
                    "Stack Overflow at PC=0x%03X\n",
                    instruction_pc
                );

                return false;
            }

            chip8->stack[chip8->sp] = chip8->pc;
            chip8->sp++; 
            chip8->pc = decoded.nnn;

            return true;
        }
        case 0x3000:
            if (chip8->V[decoded.x] == decoded.nn) {
                chip8->pc +=2;
            }
            return true;
        case 0x4000:
            if (chip8->V[decoded.x] != decoded.nn) {
                chip8->pc += 2;
            }
            return true;
        case 0x5000:
            if (decoded.n != 0) {
                break;
            }

            if (chip8->V[decoded.x] == chip8->V[decoded.y]) {
                chip8->pc +=2;
            }
            return true;
        case 0x9000:
            if (decoded.n != 0) {
                break;
            }

            if (chip8->V[decoded.x] != chip8->V[decoded.y]) {
                chip8->pc += 2;
            }
            return true;
        case 0x6000:
            chip8->V[decoded.x] = decoded.nn;
            return true;
        case 0x7000:
           chip8->V[decoded.x] += decoded.nn;
           return true; 
        case 0x8000:
           switch (decoded.n) {
                case 0x00:
                    chip8->V[decoded.x] = chip8->V[decoded.y];
                    return true; 

                case 0x01:
                    chip8->V[decoded.x] |= chip8->V[decoded.y];
                    return true; 

                case 0x02:
                    chip8->V[decoded.x] &= chip8->V[decoded.y];
                    return true;

                case  0x03:
                    chip8->V[decoded.x] ^= chip8->V[decoded.y];
                    return true; 
                
                case 0x04: {
                    uint16_t result = (uint16_t) chip8->V[decoded.x] + chip8->V[decoded.y];
                    chip8->V[0xF] = (result > 0xFF) ? 1 : 0;
                    chip8->V[decoded.x] = (uint8_t) result;
                    return true;
                }

                case 0x05: {
                    uint8_t vx = chip8->V[decoded.x];
                    uint8_t vy = chip8->V[decoded.y];

                    chip8->V[0xF] = vx >= vy;
                    chip8->V[decoded.x] = (uint8_t)(vx - vy);

                    return true;
                }

                case 0x06: {
                    uint8_t vx = chip8->V[decoded.x];

                    chip8->V[0xF] = vx & 0x01u;
                    chip8->V[decoded.x] = (uint8_t)(vx >> 1);

                    return true;
                }

                case 0x07: {
                    uint8_t vx = chip8->V[decoded.x];
                    uint8_t vy = chip8->V[decoded.y];

                    chip8->V[0xF] = vy >= vx;
                    chip8->V[decoded.x] = (uint8_t)(vy - vx); 

                    return true;
                }

                case 0xE: {
                    uint8_t vx = chip8->V[decoded.x];

                    chip8->V[0xF] = (vx >> 7) & 0x01u;
                    chip8->V[decoded.x] = (uint8_t)(vx << 1);

                    return true;
                }
           }
        case 0xA000:
           chip8->I = decoded.nnn;
           return true;
        case 0xF000:
            switch (opcode & 0x00FFu) {
                case 0x1E:
                    chip8->I = (uint16_t)(chip8->I + chip8->V[decoded.x]);
                return true;
                case 0x33: {
                    uint8_t value = chip8->V[decoded.x];

                    chip8->memory[chip8->I] = (uint8_t)(value / 100u);
                    chip8->memory[chip8->I + 1u] = (uint8_t)((value / 10u) % 10u);
                    chip8->memory[chip8->I + 2u] = (uint8_t)(value % 10u);
                    return true;
                } 
                case 0x55: {
                    size_t count = (size_t)(decoded.x + 1u);

                    if (!chip8_memory_range_valid(chip8->I, count)) {
                        fprintf(
                            stderr,
                            "Memory write out of bounds at PC=0x%03X\n",
                            (unsigned int) instruction_pc
                        );

                        return false;
                    }

                    for (size_t i=0; i < count; ++i) {
                        chip8->memory[(size_t)chip8->I + i] = chip8->V[i];
                    }

                return true;
                }
                case 0x65: {
                    size_t count = (size_t)(decoded.x) + 1u;
                    
                    if (!chip8_memory_range_valid(chip8->I, count)) {
                        fprintf(
                            stderr,
                            "Memory read out of bounds at PC=0x%03X",
                            (unsigned int)instruction_pc
                        );

                        return false;
                    }

                    for (size_t i=0; i < count; ++i) {
                        chip8->V[i] = chip8->memory[(size_t)chip8->I + i];
                    }

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

static bool chip8_memory_range_valid(uint16_t start, size_t length)
{
    if (start >= CHIP8_MEMORY_SIZE) {
        return false;
    }

    return length <= CHIP8_MEMORY_SIZE - start;
}

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
    printf("PC=0x%03X I=0x%03X SP=%u\n", chip8->pc, chip8->I, chip8->sp);

    for (int i = 0; i<16; i++) {
        printf(" V%X=0x%02X%s ",
            i,
            chip8->V[i],
            (i == 15) ? "\n" : ""
        );
    }

    printf("Stack:");

    for (unsigned int i=0; i < chip8->sp; ++i) {
        printf(" [0x%03x]", (unsigned int)(chip8->stack[i]));
    }

    printf("\n");
}

void chip8_memory_dump(const Chip8 *chip8, uint16_t start, size_t length) 
{
    if (!chip8_memory_range_valid(start, length)) {
        printf("Invalid memory range\n");
        return;
    }

    for (size_t i = 0; i<length; ++i) {
        printf(
            "0x%03X: 0x%02X\n",
            (unsigned int) ((size_t) start + i),
            (unsigned int) chip8->memory[(size_t) start + i]
        );
    }
}
