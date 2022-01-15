#include <string.h>
#include "bitops.h"
#include "layout.h"
#include "state.h"


bool State_is_start_phase(const struct State *state)
{
    return state->workers[P2][0] == state->workers[P2][1];
}


int State_height_at(const struct State *state, int pos)
{
    int height;
    for (height = 4; height > 0; height--) {
        if ((1 << pos) & state->buildings[height-1]) {
            break;
        }
    }

    return height;
}


void State_derive_start_actions(struct State *state)
{
    // P1 worker placements
    if (state->workers[P1][0] == state->workers[P1][1]) {
        for (int pos1 = 0; pos1 < 25; pos1++) {
            for (int pos2 = 0; pos2 < 25; pos2++) {
                if (pos1 == pos2) continue;
                state->actions[state->action_count].source = pos1;
                state->actions[state->action_count].dest = pos2;
                state->actions[state->action_count++].build = PLACE;
            }
        }
        return;
    }

    // P2 worker placements
    if (state->workers[P2][0] == state->workers[P2][1]) {
        for (int pos1 = 0; pos1 < 25; pos1++) {
            if (pos1 == state->workers[P1][0] ||
                    pos1 == state->workers[P1][1]) {
                continue;
            }
            for (int pos2 = 0; pos2 < 25; pos2++) {
                if (pos1 == pos2) continue;
                if (pos2 == state->workers[P1][0] ||
                        pos2 == state->workers[P1][1]) {
                    continue;
                }

                state->actions[state->action_count].source = pos1;
                state->actions[state->action_count].dest = pos2;
                state->actions[state->action_count++].build = PLACE;
            }
        }
    }
}


void State_derive_actions(struct State *state)
{
    state->action_count = 0;

    if (State_is_start_phase(state)) {
        State_derive_start_actions(state);
        return;
    }

    uint_fast32_t blocked_for_all = state->buildings[3];
    for (int w = 0; w < 4; w++) {
        blocked_for_all |= 1 << state->workers[0][w];
    }

    for (int w = 0; w < 2; w++) {
        int pos = state->workers[state->turn][w];
        int height = State_height_at(state, pos);

        // Can move to adjacent spaces with height_at <= height+1
        uint_fast32_t blocked_for_move = blocked_for_all;
        for (int h = height+2; h < 3; h++) {
            blocked_for_move |= state->buildings[h];
        }

        uint_fast32_t moves = ADJACENT_SPACES[pos] & ~blocked_for_move;

        while (moves) {
            int dest = bitscan(moves);
            moves ^= 1 << dest;

            if (State_height_at(state, dest) == 3) {
                state->actions[state->action_count].source = pos;
                state->actions[state->action_count].dest = dest;
                state->actions[state->action_count++].build = WIN;
                continue;
            }

            uint_fast32_t builds = ADJACENT_SPACES[dest] & ~blocked_for_all;
            // Can build on where we just were
            builds |= 1 << pos;

            while (builds) {
                int build = bitscan(builds);
                builds ^= 1 << build;

                state->actions[state->action_count].source = pos;
                state->actions[state->action_count].dest = dest;
                state->actions[state->action_count++].build = build;
            }
        }
    }
}


void State_derive(struct State *state)
{
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
