#include <stdio.h>
#include <string.h>
#include "bitops.h"
#include "layout.h"
#include "state.h"


bool State_is_start_phase(const struct State *state);
int State_height_at(const struct State *state, int pos);


int main()
{
    printf("Erastus tests\n");

    struct State state;
    char action_string[ACTION_STRING_SIZE];


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


    // Start phase calculation
    {
        State_new(&state);
        if (!State_is_start_phase(&state)) {
            printf("Think start phase isn't start phase\n");
        }
        state.workers[P1][0] = 0;
        state.workers[P1][1] = 1;
        state.workers[P2][0] = 2;
        state.workers[P2][1] = 4;
        if (State_is_start_phase(&state)) {
            printf("Think not start phase is start phase\n");
        }
    }


    // Start phase and basic actions
    {
        State_new(&state);
        if (state.action_count != 600) {
            printf("Incorret number of P1 start places: %d\n", state.action_count);
        }

        State_act(&state, &state.actions[0]);
        if (state.action_count != 506) {
            printf("Incorret number of P2 start places: %d\n", state.action_count);
        }

        State_act(&state, &state.actions[0]);
        if (State_is_start_phase(&state)) {
            printf("Start phase after both sides have placed");
        }

        if (state.action_count != 26) {
            printf("Incorrect number of moves after start places");
        }
    }


    // Basic win
    {
        State_new(&state);
        for (int i = 0; i < 4; i++) {
            state.workers[0][i] = i;
        }
        state.buildings[1] = 1;
        state.buildings[2] = 1 << 5;

        State_derive(&state);

        bool win_found = false;
        for (int i = 0; i < state.action_count; i++) {
            Action_to_string(&state.actions[i], action_string);

            if (state.actions[i].build == WIN) {
                if (state.actions[i].source != 0 || state.actions[i].dest != 5) {
                    printf("Win found, but with wrong move");
                }
                win_found = true;
            }
        }
        if (!win_found) {
            printf("No win found on basic win test\n");
        }
    }


    // coords_to_spacei
    {
        char coords[] = "a1";
        if (coords_to_spacei(coords) != 0) {
            printf("incorrect coords for %s\n", coords);
        }
        strcpy(coords, "a2");
        if (coords_to_spacei(coords) != 1) {
            printf("incorrect coords for %s\n", coords);
        }
        strcpy(coords, "B1");
        if (coords_to_spacei(coords) != 5) {
            printf("incorrect coords for %s\n", coords);
        }
        strcpy(coords, "e5");
        if (coords_to_spacei(coords) != 24) {
            printf("incorrect coords for %s\n", coords);
        }
    }


    // spacei_to_coords
    {
        char coords[] = "a1";
        spacei_to_coords(0, coords);
        if (strcmp(coords, "a1") != 0) {
            printf("incorrect coords for spacei 0: %s\n", coords);
        }
        spacei_to_coords(5, coords);
        if (strcmp(coords, "b1") != 0) {
            printf("incorrect coords for spacei 5: %s\n", coords);
        }
        spacei_to_coords(24, coords);
        if (strcmp(coords, "e5") != 0) {
            printf("incorrect coords for spacei 24: %s\n", coords);
        }
    }


    printf("Done!\n");
    return 0;
}
