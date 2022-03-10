#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include "bitops.h"
#include "layout.h"
#include "state.h"


bool State_is_start_phase(const struct State *state);
int State_height_at(const struct State *state, int pos);
int coords_to_spacei(const char* coords);
void spacei_to_coords(int spacei, char* coords);


int main()
{
    printf("Erastus tests\n");

    time_t seed = time(NULL);
    printf("Seed:\t%ld\n", seed);
    srand(seed);

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
            printf("incorrect spacei for %s\n", coords);
        }
        strcpy(coords, "a2");
        if (coords_to_spacei(coords) != 5) {
            printf("incorrect spacei for %s\n", coords);
        }
        strcpy(coords, "B1");
        if (coords_to_spacei(coords) != 1) {
            printf("incorrect spacei for %s\n", coords);
        }
        strcpy(coords, "e5");
        if (coords_to_spacei(coords) != 24) {
            printf("incorrect spacei for %s\n", coords);
        }
        strcpy(coords, "xx");
        if (coords_to_spacei(coords) != PLACE) {
            printf("incorrect coords for %s\n", coords);
        }
    }


    // spacei_to_coords
    {
        char coords[COORDS_STRING_SIZE];
        spacei_to_coords(0, coords);
        if (strcmp(coords, "a1") != 0) {
            printf("incorrect coords for spacei 0: %s\n", coords);
        }
        spacei_to_coords(1, coords);
        if (strcmp(coords, "b1") != 0) {
            printf("incorrect coords for spacei 1: %s\n", coords);
        }
        spacei_to_coords(24, coords);
        if (strcmp(coords, "e5") != 0) {
            printf("incorrect coords for spacei 24: %s\n", coords);
        }
        spacei_to_coords(PLACE, coords);
        if (strcmp(coords, "xx") != 0) {
            printf("incorrect coords for spacei PLACE: %s\n", coords);
        }
    }


    // Initial state to string
    {
        State_new(&state);
        char string[STATE_STRING_SIZE];

        State_to_string(&state, string);
        if (strcmp(string, "0000000000000000000000000xxxxxxxx1") != 0) {
            printf("incorrect state string: %s\n", string);
        }
    }


    // To string and back for randomly-walked game
    {
        State_new(&state);
        while (state.action_count) {
            char string[STATE_STRING_SIZE];
            State_to_string(&state, string);

            struct State state_from_string;
            State_from_string(&state_from_string, string);

            if (memcmp(&state, &state_from_string, sizeof(struct State))) {
                printf("deserialized to different state!\n");
                State_print_debug(&state);
                printf("\nstring:\t%s\n\n", string);
                State_print_debug(&state_from_string);

                printf("action differences:\n");
                for (int i = 0; i < state.action_count; i++) {
                    if (memcmp(&state.actions[i], &state_from_string.actions[i],
                            sizeof(struct Action))) {
                        printf("(%d, %d, %d),\t(%d, %d, %d)\n",
                            state.actions[i].source,
                            state.actions[i].dest,
                            state.actions[i].build,
                            state_from_string.actions[i].source,
                            state_from_string.actions[i].dest,
                            state_from_string.actions[i].build
                        );
                    }
                }
            }

            struct Action action = state.actions[rand() % state.action_count];
            State_act(&state, &action);
        }
    }


    // Action serialization and back throughout a random walk
    {
        State_new(&state);
        while (state.action_count) {
            struct Action action = state.actions[rand() % state.action_count];

            char action_string[ACTION_STRING_SIZE];
            Action_to_string(&action, action_string);

            struct Action action_from_string;
            Action_from_string(&action_from_string, action_string);

            if (memcmp(&action, &action_from_string, sizeof(struct Action))) {
                printf("serialization and back changes action %s\n", action_string);
                Action_print(&action);
                Action_print(&action_from_string);
            }

            State_act(&state, &action);
        }
    }


    // Basic State_check_action
    {
        State_new(&state);
        struct Action action;
        char bad_string[] = "a13b2";
        Action_from_string(&action, bad_string);
        if (State_check_action(&state, &action)) {
            printf("State_check_action passes an invalid action\n");
        }
        char good_string[ACTION_STRING_SIZE] = "a1&b2";
        Action_from_string(&action, good_string);
        if (!State_check_action(&state, &action)) {
            printf("State_check_action fails valid action %s\n", good_string);
            Action_print(&action);
        }
    }


    printf("Done!\n");
    return 0;
}
