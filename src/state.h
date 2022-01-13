#ifndef STATE_H
#define STATE_H


#include <stdbool.h>
#include <stdint.h>


#define MAX_ACTIONS 128


enum Player {PLAYER_1=0, PLAYER_2};


struct Action {
    uint8_t source;
    uint8_t dest;
    uint8_t build;
};


struct State {
    // Core information
    uint_fast32_t buildings[4];
    uint_fast8_t workers[2][2];
    enum Player turn;

    // Derived information
    struct Action actions[MAX_ACTIONS];
    uint_fast8_t actionCount;
    uint_fast8_t worker_heights[2][2];
};


void State_new_game(struct State *state);
void State_derive(struct State *state);

void State_print(const struct State *state);
void State_printDebug(const struct State *state);

#endif
