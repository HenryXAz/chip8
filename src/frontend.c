#include "frontend.h"
#include <SDL3/SDL.h>

#include <stdio.h>
#include <stdlib.h>

#define FRONTEND_SCALE 10
#define FRONTEND_WIDTH (64 * FRONTEND_SCALE)
#define  FRONTEND_HEIGHT (32 * FRONTEND_SCALE)


struct Frontend {
    SDL_Window *window;
    SDL_Renderer *renderer;
};

Frontend *frontend_create(void) {
    if (!SDL_Init(SDL_INIT_VIDEO)) {
        fprintf(stderr, "Could not initialize SDL: %s\n", SDL_GetError());
        return NULL;
    }

    Frontend *frontend = calloc(0x01, sizeof(*frontend));

    if (frontend == NULL) {
        fprintf(stderr, "Could not allocate frontend \n");

        SDL_Quit();
        return NULL;
    }

    frontend->window = SDL_CreateWindow("CHIP8 Emulator", FRONTEND_WIDTH, FRONTEND_HEIGHT, 0);

    if (frontend->window == NULL) {
        fprintf(stderr, "Could not create SDL window: %s \n", SDL_GetError());
        SDL_GetError();

        free(frontend);
        SDL_Quit();
        return NULL;
    }

    frontend->renderer = SDL_CreateRenderer(frontend->window, NULL);

    if (frontend->renderer == NULL) {
        fprintf(stderr, "Could not create SDL renderer: %s\n", SDL_GetError());
        SDL_GetError();

        SDL_DestroyWindow(frontend->window);
        free(frontend);
        SDL_Quit();

        return NULL;
    }

    

    return frontend;
}

void frontend_destroy(Frontend *frontend)
{
    if (frontend == NULL) {
        return;
    }

    if (frontend->renderer != NULL) {
        SDL_DestroyRenderer(frontend->renderer);
    }

    if (frontend->window != NULL) {
        SDL_DestroyWindow(frontend->window);
    }

    free(frontend);
    SDL_Quit();
}

bool frontend_render(Frontend *frontend, const Chip8 *chip8) {
    if (!SDL_SetRenderDrawColor(
        frontend->renderer,
        0u,
        0u,
        0u,
        255u
    )) {
        fprintf(stderr, "Could not set render color: %s\n", SDL_GetError());
        return false;
    }

    if (!SDL_RenderClear(frontend->renderer)) {
        fprintf(stderr, "Could not clear renderer: %s\n", SDL_GetError());
        return false;
    }

    if (!SDL_SetRenderDrawColor(
        frontend->renderer,
        255u,
        255u,
        255u,
        255u
    )) {
        return false;
    }

    for (unsigned int y = 0; y < CHIP8_DISPLAY_HEIGHT; ++y) {
        for (unsigned int x = 0; x < CHIP8_DISPLAY_WIDTH; ++x) {
            size_t index = (size_t) y * CHIP8_DISPLAY_WIDTH + x;

            if (chip8->display[index] == 0x00) {
                continue;
            }

            SDL_FRect rect = {
                .x = (float)(x * FRONTEND_SCALE),
                .y = (float)(y * FRONTEND_SCALE),
                .w = (float) FRONTEND_SCALE,
                .h = (float) FRONTEND_SCALE
            };

            if (!SDL_RenderFillRect(frontend->renderer, &rect)) {
                fprintf(stderr, "Could not draw pixel: %s\n", SDL_GetError());
                return false;
            }

        }
    }


    // SDL_FRect rect = {
    //     .x = 50.0f,
    //     .y = 30.0f,
    //     .w = 100.0f,
    //     .h = 100.0f
    // };

    // if (!SDL_RenderFillRect(
    //     frontend->renderer,
    //     &rect
    // )) {
    //     fprintf(
    //         stderr,
    //         "Could not draw rectangle: %s\n",
    //         SDL_GetError()
    //     );

    //     return false;
    // }
    
    if (!SDL_RenderPresent(frontend->renderer)) {
        fprintf(stderr, "Could not present renderer: %s\n", SDL_GetError());
        return false;
    }

    return true;
}

bool frontend_process_events(Frontend * frontend) 
{
    (void) frontend;

    SDL_Event event;

    while( SDL_PollEvent(&event)) {
        if (event.type == SDL_EVENT_QUIT) {
            return false;
        }
    }

    return true;
}
