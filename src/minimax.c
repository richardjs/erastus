#include <math.h>
#include <string.h>
#include "minimax.h"


static struct MinimaxOptions options;
static struct MinimaxResults *results;


void MinimaxOptions_default(struct MinimaxOptions *options) {
    options->depth = 3;
    options->skip_player = -1;
}


float evaluate(const struct State *state) {
    return 0.0;
}


float search(const struct State *state, int depth) {
    results->stats.nodes++;

    if (state->actions[0].build == WIN) {
        return depth;
    }

    if (state->action_count == 0) {
        return -depth;
    }

    if (depth == 0) {
        results->stats.leaves++;
        return evaluate(state);
    }

    if (state->turn == options.skip_player) {
       struct State child = *state;
       child.turn = !state->turn;
       State_derive(&child);
       return -search(&child, depth - 1);
    }

    float best_score = -INFINITY;
    for (int i = 0; i < state->action_count; i++) {
        struct State child = *state;
        State_act(&child, &state->actions[i]);
        float child_score = -search(&child, depth - 1);
        if (child_score > best_score) {
           best_score = child_score;
        }
    }

    return best_score;
}


void minimax(const struct State *state, struct MinimaxResults *r, const struct MinimaxOptions *o) {
    if (o == NULL) {
        MinimaxOptions_default(&options);
    } else {
        options = *o;
    }

    results = r;
    memset(results, 0, sizeof(struct MinimaxResults));

    results->stats.nodes++;

    if (state->actions[0].build == WIN) {
        results->actioni = 0;
        results->score = INFINITY;
        results->stats.leaves++;
        return;
    }

    if (state->action_count == 0) {
        results->score = -INFINITY;
        results->stats.leaves++;
        return;
    }

    if (options.depth == 0) {
        results->score = evaluate(state);
        results->stats.leaves++;
        return;
    }


    results->score = -INFINITY;
    for (int i = 0; i < state->action_count; i++) {
        struct State child = *state;
        State_act(&child, &state->actions[i]);
        float child_score = -search(&child, options.depth - 1);
        if (child_score > results->score) {
           results->score = child_score;
           results->actioni = i;
        }
    }
}
