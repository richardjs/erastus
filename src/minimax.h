#ifndef MINIMAX_H
#define MINIMAX_H


#include <stdint.h>
#include "state.h"


struct MinimaxOptions {
    int depth;
    enum Player skip_player;
};


struct MinimaxStats {
    uint64_t nodes;
    uint64_t leaves;
};


struct MinimaxResults {
    uint16_t actioni;
    float score;
    struct MinimaxStats stats;
};


void MinimaxOptions_default(struct MinimaxOptions*);

void minimax(const struct State *state, struct MinimaxResults *r, const struct MinimaxOptions *o);

#endif
