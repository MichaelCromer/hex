#ifndef WRITE_H
#define WRITE_H

#include "state.h"

void write_state(FILE *file, struct State *state);
void read_state(FILE *file, struct State *state);

#endif
