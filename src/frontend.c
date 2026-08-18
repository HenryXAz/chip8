#include "frontend.h"
#include <SDL3/SDL.h>

#include <stdio.h>
#include <stdlib.h>

#define FRONTEND_SCALE 10
#define FRONTEND_WIDTH (64 * FRONTEND_SCALE)
#define  FRONTEND_HEIGHT (32 * FRONTEND_SCALE)


struct Frontend {
    SDL_Window * window;
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

    return frontend;
}

void frontend_destroy(Frontend *frontend)
{
    if (frontend == NULL) {
        return;
    }

    if (frontend->window != NULL) {
        SDL_DestroyWindow(
            frontend->window
        );
    }

    free(frontend);
    SDL_Quit();
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
