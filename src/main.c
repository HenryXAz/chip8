#include <stdio.h>
#include <stdlib.h>
#include "chip8.h"

int main(int argc, char* argv[]) 
{
    if (argc != 2) {
        fprintf(
            stderr,
            "Usage:%s <rom-file>\n",
            argv[0]
        );
        
        return 1;
    }

    const char *rom_path = argv[1];
   
    FILE *file = fopen(rom_path, "rb");     

    if (file == NULL) {
        fprintf(
            stderr,
            "Could not open ROM: %s\n",
            rom_path
        );

        return 1;
    }

    if (fseek(file, 0L, SEEK_END) != 0) {
        fprintf(
            stderr,
            "Could not seek ROM\n"
        );

        fclose(file);
        return 1;
    }

    long file_size = ftell(file);

    if (file_size < 0L) {
        fprintf(
            stderr,
            "Could not determine size ROM \n"
        );
        fclose(file);

        return 1;
    }

    const long max_rom_size = (long)(CHIP8_MEMORY_SIZE - CHIP8_PROGRAM_START);

    if (file_size > max_rom_size) {
        fprintf(
            stderr,
            "ROM is too large: %ld bytes "
            "(maximum %ld bytes)\n",
            file_size,
            max_rom_size
        );
        fclose(file);

        return 1;
    }

    if (fseek(file, 0L, SEEK_SET) != 0) {
        fprintf(
            stderr,
            "Could not rewind ROM\n"
        );
        fclose(file);

        return 1;
    }
    
    if (file_size == 0L) {
        fprintf(stderr, "ROM is empty\n");
        fclose(file);
        return 1;
    }

    uint8_t *rom = malloc((size_t) file_size);

    if (rom == NULL) {
        fprintf(stderr, "Could not allocate ROM buffer\n");
        fclose(file);

        return 1;
    }

    size_t bytes_read = fread(rom, 1, (size_t) file_size, file);

    if (bytes_read != (size_t)file_size) {
        fprintf(
            stderr,
            "Could not read complete rom "
            "expected %ld bytes, read %zu\n",
            file_size,
            bytes_read
        );

        free(rom);    
        fclose(file);

        return 1;
    } 
  
    printf("ROM size: %ld bytes \n", file_size);
    
    printf(
        "Successfully opened ROM: %s\n",
        rom_path
    );

    fclose(file);

    Chip8 chip8;
    chip8_init(&chip8);    

    if (!chip8_load_program(&chip8, rom, sizeof(rom))) {
        fprintf(stderr, "Program is too large to fit in memory\n");
        free(rom);

        return 1;
    }

    chip8_memory_dump(
        &chip8,
        CHIP8_PROGRAM_START,
        (size_t)file_size
    );

    for (int i = 0; i < 10u; i++) {
        chip8_dump_state(&chip8);

        if(!chip8_cycle(&chip8)) {
            break;
        }
    }

    chip8_dump_state(&chip8);
    chip8_memory_dump(&chip8, 0x350, 3);

    return 0;    
}
