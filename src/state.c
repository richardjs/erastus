#include <stdlib.h>
#include <string.h>
#include "bitops.h"
#include "layout.h"
#include "state.h"


// TODO would it be worth it to put this in a lookup table?
uint8_t space_distance(uint8_t x, uint8_t y) {
    uint8_t dx = abs((x % 5) - (y % 5));
    uint8_t dy = abs((x / 5) - (y / 5));
    return dx > dy ? dx : dy;
}


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


// Note: technically this won't detect if opponent has a faster win
bool State_unstoppable_win(const struct State *state) {
    // TODO store this on state?
    uint_fast32_t worker_bitboard = 0;
    for (int w = 0; w < 4; w++) {
        worker_bitboard |= (1 << state->workers[0][w]);
    }

    // Check each worker
    for (int w = 0; w < 2; w++) {
        uint_fast8_t worker_space = state->workers[state->turn][w];

        if (State_height_at(state, worker_space) == 2) {
            // TODO we could potentially benefit from bitboard of height 2 buildings
            bool adjacent_2 = false;
            uint_fast32_t adjacents = ADJACENT_SPACES[worker_space];
            adjacents &= (~worker_bitboard);
            while (adjacents) {
                int adjacent = bitscan(adjacents);
                adjacents ^= 1 << adjacent;

                if (State_height_at(state, adjacent) == 2) {
                    adjacent_2 = true;
                    break;
                }
            }

            if (!adjacent_2) {
                continue;
            }

            bool nearby_opponent_worker = false;
            for (int o = 0; o < 2; o++) {
                int opponent_space = state->workers[!state->turn][o];
                int distance = space_distance(worker_space, opponent_space);
                if (distance <= 2) {
                    /*
                    if (distance == 1) {
                        nearby_opponent_worker = true;
                        break;
                    }

                    // Test for interposing worker between space and opponent
                    int other_worker_space = state->workers[state->turn][!w];
                    if (other_worker_space == (worker_space + opponent_space) / 2) {
                        continue;
                    }
                    */

                    nearby_opponent_worker = true;
                    break;
                }
            }

            if (!nearby_opponent_worker) {
                return true;
            }
        }
    }

    return false;
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
    // TODO We only need to do this for memcmp purposes in test.c
    memset(&state->actions, 0, sizeof(struct Action) * MAX_ACTIONS);

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
        for (int h = height+1; h < 3; h++) {
            blocked_for_move |= state->buildings[h];
        }

        uint_fast32_t moves = ADJACENT_SPACES[pos] & ~blocked_for_move;

        while (moves) {
            int dest = bitscan(moves);
            moves ^= 1 << dest;

            if (State_height_at(state, dest) == 3) {
                // If we find a win, store it as the first action for
                // quick checking later
                if (state->actions[0].build != WIN) {
                    state->actions[state->action_count++] = state->actions[0];
                    state->actions[0].source = pos;
                    state->actions[0].dest = dest;
                    state->actions[0].build = WIN;
                } else {
                    state->actions[state->action_count].source = pos;
                    state->actions[state->action_count].dest = dest;
                    state->actions[state->action_count++].build = WIN;
                }
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


bool State_check_action(const struct State *state, const struct Action *action)
{
    for (int i = 0; i < state->action_count; i++) {
        if (!memcmp(&state->actions[i], action, sizeof(struct Action))) {
            return true;
        }
    }

    return false;
}


bool State_check_win(const struct State *state)
{
    for (int w = 0; w < 4; w++) {
        int pos = state->workers[0][w];
        if (State_height_at(state, pos) == 3) {
            return true;
        }
    }

    return false;
}


void State_act(struct State *state, const struct Action *action)
{
    if (action->build == PLACE) {
        state->workers[state->turn][0] = action->source;
        state->workers[state->turn][1] = action->dest;
    } else {
        // TODO we might could optimize this to save the reference to the worker
        // and also maybe even not bother with turn, just looking for worker that
        // matches action->source
        int w;
        for (w = 0; w < 2; w++) {
            if (state->workers[state->turn][w] == action->source) break;
        }
        state->workers[state->turn][w] = action->dest;
        if (action->build != WIN) {
            int height = State_height_at(state, action->build);
            state->buildings[height] |= 1 << action->build;
        }
    }

    state->turn = !state->turn;
    State_derive(state);
}


void State_new(struct State *state)
{
    memset(state, 0, sizeof(struct State));
    memset(state->workers, PLACE, 4);
    State_derive(state);
}
