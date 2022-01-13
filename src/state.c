#include "string.h"

#include "state.h"


bool State_is_start_phase(const struct State *state)
{
    return state->workers[PLAYER_2][0] == state->workers[PLAYER_2][1];
}


int State_height_at(const struct State *state, int pos)
{
    int height;
    for (height = 4; height > 0; height--) {
        if ((1<<pos) & state->buildings[height-1]) {
            break;
        }
    }

    return height;
}


void State_derive_worker_heights(struct State *state)
{
    if (State_is_start_phase(state)) {
        return;
    }

    for (enum Player p = 0; p < 2; p++) {
        for (int w = 0; w < 2; w++) {
            int pos = state->workers[p][w];
            state->worker_heights[p][w] = State_height_at(state, pos);
        }
    }
}


void State_derive(struct State *state)
{
    State_derive_worker_heights(state);
}


void State_new_game(struct State *state)
{
    memset(state, 0, sizeof(struct State));
    State_derive(state);
}
