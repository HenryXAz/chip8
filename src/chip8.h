#ifndef CHIP8_H
#define CHIP8_H

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

#define CHIP8_MEMORY_SIZE 4096
#define CHIP8_REGISTER_COUNT 16
#define CHIP8_STACK_SIZE 16
#define CHIP8_DISPLAY_WIDTH 64
#define CHIP8_DISPLAY_HEIGHT 32
#define CHIP8_PROGRAM_START 0x200
#define CHIP8_FONT_START 0x050
#define CHIP8_FONT_BYTES_PER_CHAR 0x05

typedef struct {
    uint8_t memory[CHIP8_MEMORY_SIZE];

    uint8_t V[CHIP8_REGISTER_COUNT]; 

    uint16_t I;
    uint16_t pc;

    uint16_t stack[CHIP8_STACK_SIZE];
    uint8_t sp;

    uint8_t delay_timer;
    uint8_t sound_timer;

    uint8_t display[CHIP8_DISPLAY_HEIGHT * CHIP8_DISPLAY_WIDTH];
    uint8_t keypad[16];

    bool halted;
} Chip8;

//  Chip8
void chip8_init(Chip8 *chip8);

bool chip8_load_program(Chip8 *chip8, const uint8_t *program, size_t size);

bool chip8_cycle(Chip8 *chip8);

void chip8_tick_timers(Chip8 *chip8);

void chip8_dump_state(const Chip8 *chip8);
void chip8_memory_dump(const Chip8 *chip8, uint16_t start, size_t length);
void chip8_dump_display(const Chip8 *chip8);
void chip8_dump_keypad(const Chip8 *chip8);

#endif
