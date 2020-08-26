#ifndef __AI__
#define __AI__

#include <stdint.h>
#include <unistd.h>
#include "node.h"
#include "priority_queue.h"

typedef struct output_s output_t;

extern int GlobalDepth;
extern int GlobalGenNodes;
extern int GlobalExpNodes;
extern double GlobalSecs;

void initialize_ai();

move_t get_next_move( state_t init_state, int budget, propagation_t propagation, char* stats );

#endif
