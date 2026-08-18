#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include "chip8.h"
#include "frontend.h"

static double current_time_seconds(void) {
    struct timespec ts;
    
    if (timespec_get(&ts, TIME_UTC) != TIME_UTC) {
        return -1.0;
    }

    return (double) ts.tv_sec + (double) ts.tv_nsec / 1000000000.0;
}

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
  
    fclose(file);

    Chip8 chip8;
    chip8_init(&chip8);
 
    Frontend *frontend = frontend_create();

    if (frontend == NULL) {
        return 1;
    }

    if (!chip8_load_program(&chip8, rom, bytes_read)) {
        fprintf(stderr, "Program is too large to fit in memory\n");
        free(rom);

        return 1;
    }

    const double cpu_frequency = 0x2bc;
    const double timer_frequency = 0x3c;
    const double render_frequency = 0x3c;

    const double cpu_interval = 1.0 / cpu_frequency;
    const double timer_interval = 1.0 / timer_frequency;
    const double render_interval = 1.0 / render_frequency;

    double cpu_accumulator = 0.00;
    double timer_accumulator = 0.00;
    double render_accumulator = 0.00;

    double previous_time = current_time_seconds();

    bool running = true;

    while (running) {
        double current_time = current_time_seconds();

        if (current_time < 0.0) {
            fprintf(stderr, "Clock error \n");
            break;
        }

        double elapsed = current_time - previous_time;

        if (elapsed > 0.1) {
            elapsed = 0.1;
        }

        previous_time = current_time;

        cpu_accumulator += elapsed;
        timer_accumulator += elapsed;
        render_accumulator += elapsed;

        /*  Host input/events */
        if (!frontend_process_events(frontend)) {
            running = false;
            break;
        }

        /* CHIP8 CPU */
        while (cpu_accumulator >= cpu_interval) {
            if (!chip8_cycle(&chip8)) {
                fprintf(stderr, "CHIP-8 execution error \n");
                running = false;

                break;
            }

            cpu_accumulator -= cpu_interval;
        }

        if (!running) {
            break;
        }

        /* CHIP-8 timers*/
        while (timer_accumulator >= timer_interval) {
            chip8_tick_timers(&chip8);
            timer_accumulator -= timer_interval;
        }

        /* Host Rendering */
        if (render_accumulator >= render_interval) {
            if (!frontend_render(frontend, &chip8)) {
                running = false;
                break;
            }

            render_accumulator -= render_interval;
        }
    }

    frontend_destroy(frontend);

    return 0;
}
