/*  crossbar_input_voq.h

    This provides an implementation of an input buffered crossbar switch input
    module, using virtual output queues to prevent head of line blocking. */

#ifndef CROSSBAR_INPUT_VOQ_H
#define CROSSBAR_INPUT_VOQ_H

#include "crossbar_components.h"

struct crossbar_input_voq;

typedef struct crossbar_input_voq *crossbar_input_voq_t;

#endif