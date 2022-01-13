#include <stdio.h>

#include "state.h"


bool State_is_start_phase(const struct State *state);
int State_height_at(const struct State *state, int pos);
void State_derive_worker_heights(struct State *state);


int main()
{
    printf("Erastus tests\n");

    struct State state;


    // Test height calculation
    {
        State_new_game(&state);
        int height = State_height_at(&state, 0);
        if (height != 0) {
            printf("Incorrect height calculation for ground: %d\n", height);
        }

        for (int i = 1; i <= 4; i++) {
            state.buildings[i-1] = 1;
            int height = State_height_at(&state, 0);
            if (height != i) {
                printf("Incorrect height calculation for stage %d: %d\n", i, height);
            }
        }
    }


    // Test worker height derivation
    {
        State_new_game(&state);
        state.workers[PLAYER_1][0] = 0;
        state.workers[PLAYER_1][1] = 4;
        state.workers[PLAYER_2][0] = 8;
        state.workers[PLAYER_2][1] = 12;
        state.buildings[0] = 1<<0;
        state.buildings[1] = 1<<4;
        state.buildings[2] = 1<<8;
        state.buildings[3] = 1<<12;

        State_derive_worker_heights(&state);
        if (state.worker_heights[PLAYER_1][0] != 1) {
            printf("Incorrect derived height for 0: %d\n", state.worker_heights[0][0]);
        }
        if (state.worker_heights[PLAYER_1][1] != 2) {
            printf("Incorrect derived height for 1: %d\n", state.worker_heights[0][1]);
        }
        if (state.worker_heights[PLAYER_2][0] != 3) {
            printf("Incorrect derived height for 2: %d\n", state.worker_heights[1][0]);
        }
        if (state.worker_heights[PLAYER_2][1] != 4) {
            printf("Incorrect derived height for 3: %d\n", state.worker_heights[1][1]);
        }
    }


    // Test start phase calculation
    {
        State_new_game(&state);
        if (!State_is_start_phase(&state)) {
            printf("Think start phase isn't start phase\n");
        }
        state.workers[PLAYER_1][0] = 0;
        state.workers[PLAYER_1][1] = 1;
        state.workers[PLAYER_2][0] = 2;
        state.workers[PLAYER_2][1] = 4;
        if (State_is_start_phase(&state)) {
            printf("Think not start phase is start phase\n");
        }
    }

    printf("Done!\n");

    return 0;
}
