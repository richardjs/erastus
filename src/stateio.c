#include <stdio.h>
#include "state.h"


int State_height_at(const struct State *state, int pos);


void State_print(const struct State *state)
{
	for (int row = 0; row < 5; row++) {
		for (int col = 0; col < 5; col++) {
            int pos = 5*row + col;

            if (State_height_at(state, pos) == 4) {
                fprintf(stderr, " /|\\ ");
                continue;
            }

            if ((state->workers[P1][0] == pos || state->workers[P1][1] == pos)
                    && state->workers[P1][0] != state->workers[P1][1]) {
                fprintf(stderr, "  X  ");
            }
            else if ((state->workers[P2][0] == pos || state->workers[P2][1] == pos)
                    && state->workers[P2][0] != state->workers[P2][1]) {
                fprintf(stderr, "  O  ");
            } else {
                fprintf(stderr, "     ");
            }

		}
        fprintf(stderr, "\n");

		for (int col = 0; col < 5; col++) {
            int pos = 5*row + col;

            switch (State_height_at(state, pos)) {
            case 0:
                fprintf(stderr, " --- ");
                break;
            case 1:
                fprintf(stderr, "  I  ");
                break;
            case 2:
                fprintf(stderr, " I I ");
                break;
            case 3:
            case 4:
                fprintf(stderr, " III ");
                break;
            }

		}
        fprintf(stderr, "\n");
	}
}

void State_printDebug(const struct State *state)
{
}
