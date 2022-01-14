#include <stdio.h>

#include "state.h"


bool State_is_start_phase(const struct State *state);
int State_height_at(const struct State *state, int pos);
void State_derive_worker_heights(struct State *state);


int main()
{
    printf("Erastus tests\n");

    struct State state;


    // Height calculation
    {
        State_new(&state);
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


    // Worker height derivation
    {
        State_new(&state);
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


    // Start phase calculation
    {
        State_new(&state);
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


    // Place actions
    {
        State_new(&state);
        if (state.actionCount != 600) {
            printf("Incorret number of P1 start places: %d\n", state.actionCount);
        }

        State_act(&state, &state.actions[0]);
        if (state.actionCount != 506) {
            printf("Incorret number of P2 start places: %d\n", state.actionCount);
        }

        State_act(&state, &state.actions[0]);
        if (State_is_start_phase(&state)) {
            printf("Start phase after both sides have placed");
        }
    }


    printf("Done!\n");

    return 0;
}
