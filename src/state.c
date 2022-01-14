#include <string.h>
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


void State_derive_start_actions(struct State *state)
{
    // P1 worker placements
    if (state->workers[PLAYER_1][0] == state->workers[PLAYER_1][1]) {
        for (int pos1 = 0; pos1 < 25; pos1++) {
            for (int pos2 = 0; pos2 < 25; pos2++) {
                if (pos1 == pos2) continue;
                state->actions[state->actionCount].source = pos1;
                state->actions[state->actionCount++].dest = pos2;
            }
        }
        return;
    }

    // P2 worker placements
    if (state->workers[PLAYER_2][0] == state->workers[PLAYER_2][1]) {
        for (int pos1 = 0; pos1 < 25; pos1++) {
            if (pos1 == state->workers[PLAYER_1][0] ||
                    pos1 == state->workers[PLAYER_1][1]) {
                continue;
            }
            for (int pos2 = 0; pos2 < 25; pos2++) {
                if (pos1 == pos2) continue;
                if (pos2 == state->workers[PLAYER_1][0] ||
                        pos2 == state->workers[PLAYER_1][1]) {
                    continue;
                }

                state->actions[state->actionCount].source = pos1;
                state->actions[state->actionCount++].dest = pos2;
            }
        }
    }
}


void State_derive_actions(struct State *state)
{
    state->actionCount = 0;

    if (State_is_start_phase(state)) {
        State_derive_start_actions(state);
        return;
    }
}


void State_derive(struct State *state)
{
    State_derive_worker_heights(state);
    State_derive_actions(state);
}


void State_act(struct State *state, const struct Action *action)
{
    if (State_is_start_phase(state)) {
        state->workers[state->turn][0] = action->source;
        state->workers[state->turn][1] = action->dest;

        state->turn = !state->turn;

        State_derive(state);

        return;
    }
}


void State_new(struct State *state)
{
    memset(state, 0, sizeof(struct State));
    State_derive(state);
}
