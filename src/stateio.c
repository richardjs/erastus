#include <stdio.h>
#include "state.h"


const char SPACE_STRINGS[25][3] = {
    "a5", "b5", "c5", "d5", "e5",
    "a4", "b4", "c4", "d4", "e4",
    "a3", "b3", "c3", "d3", "e3",
    "a2", "b2", "c2", "d2", "e2",
    "a1", "b1", "c1", "d1", "e1",
};


bool State_is_start_phase(const struct State *state);
int State_height_at(const struct State *state, int pos);


void State_print(const struct State *state)
{
    fprintf(stderr, "| - - + - - + - - + - - + - - |\n");

	for (int row = 0; row < 5; row++) {
		for (int col = 0; col < 5; col++) {
            int pos = 5*row + col;

            if (State_height_at(state, pos) == 4) {
                fprintf(stderr, "| === ");
                continue;
            }

            if ((state->workers[P1][0] == pos || state->workers[P1][1] == pos)
                    && state->workers[P1][0] != state->workers[P1][1]) {
                fprintf(stderr, "|  X  ");
            }
            else if ((state->workers[P2][0] == pos || state->workers[P2][1] == pos)
                    && state->workers[P2][0] != state->workers[P2][1]) {
                fprintf(stderr, "|  O  ");
            } else {
                fprintf(stderr, "|     ");
            }

		}
        fprintf(stderr, "|\n");

		for (int col = 0; col < 5; col++) {
            int pos = 5*row + col;

            switch (State_height_at(state, pos)) {
            case 0:
                fprintf(stderr, "|     ");
                break;
            case 1:
                fprintf(stderr, "| I   ");
                break;
            case 2:
                fprintf(stderr, "| II  ");
                break;
            case 3:
            case 4:
                fprintf(stderr, "| III ");
                break;
            }
		}
        fprintf(stderr, "|\n");

        fprintf(stderr, "| - - + - - + - - + - - + - - |\n");
	}
}


void State_print_debug(const struct State *state)
{
    State_print(state);
    fprintf(stderr, "Start:\t\t%s\n", State_is_start_phase(state) ? "Y" : "N");
    fprintf(stderr, "Actions:\t%d\n", state->action_count);
}


void Action_to_string(const struct Action *action, char *string)
{
    const char *source = SPACE_STRINGS[action->source];
    const char *dest = SPACE_STRINGS[action->dest];
    const char *build = SPACE_STRINGS[action->build];
    if (action->build == PLACE) {
        sprintf(string, "%s&%s", source, dest, build);
    } else if (action->build == WIN) {
        sprintf(string, "%s-%s#", source, dest);
    } else {
        sprintf(string, "%s-%s+%s", source, dest, build);
    }
}
