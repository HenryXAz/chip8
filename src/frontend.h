
#ifndef FRONTEND_H
#define FRONTEND_H

#include <stdbool.h>
#include "chip8.h"

typedef struct Frontend Frontend;

Frontend *frontend_create(void);
bool frontend_process_events(Frontend *frontend);

bool frontend_init(Frontend *frontend);

bool frontend_render(Frontend *frontend);

void frontend_destroy(Frontend *frontend);

#endif
