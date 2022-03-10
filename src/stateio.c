#include <ctype.h>
#include <stdio.h>
#include "state.h"


bool State_is_start_phase(const struct State *state);
int State_height_at(const struct State *state, int pos);


int coords_to_spacei(const char* coords)
{
    if (tolower(coords[0]) == 'x') {
        return PLACE;
    }
    return 5*(tolower(coords[0])-'a') + coords[1] - '1';
}


void spacei_to_coords(int spacei, char *coords)
{
    if (spacei == PLACE) {
        coords[0] = 'x';
        coords[1] = 'x';
        coords[2] = '\0';
        return;
    }
    coords[0] = 'a' + (spacei/5);
    coords[1] = '1' + (spacei % 5);
    coords[2] = '\0';
}


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

    fprintf(stderr, "\nBuilding bitboards:\t%x\t%x\t%x\t%x\n",
        state->buildings[0], state->buildings[1],
        state->buildings[2], state->buildings[3]
    );
    fprintf(stderr, "Workers:\t\t%d\t%d\t%d\t%d\n",
        state->workers[0][0], state->workers[0][1],
        state->workers[1][0], state->workers[1][2]
    );
}


void Action_print(const struct Action *action)
{
    fprintf(stderr, "Action (%d, %d, %d)\n",
        action->source, action->dest, action->build);
}


void Action_from_string(struct Action *action, const char *string)
{
    action->source = coords_to_spacei(&string[0]);
    action->dest = coords_to_spacei(&string[3]);
    if (string[2] == '&') {
        action->build = PLACE;
    } else if (string[5] == '#' ) {
        action->build = WIN;
    } else {
        action->build = coords_to_spacei(&string[6]);
    }
}


void Action_to_string(const struct Action *action, char *string)
{
    char source[COORDS_STRING_SIZE];
    spacei_to_coords(action->source, source);
    char dest[COORDS_STRING_SIZE];
    spacei_to_coords(action->dest, dest);
    char build[COORDS_STRING_SIZE];
    spacei_to_coords(action->build, build);

    if (action->build == PLACE) {
        sprintf(string, "%s&%s", source, dest, build);
    } else if (action->build == WIN) {
        sprintf(string, "%s-%s#", source, dest);
    } else {
        sprintf(string, "%s-%s+%s", source, dest, build);
    }
}


void State_from_string(struct State *state, const char* string)
{
    State_new(state);

    for (int i = 0; i < 25; i++) {
        switch (string[i]) {
            case '4':
                state->buildings[3] |= 1<<i;
            case '3':
                state->buildings[2] |= 1<<i;
            case '2':
                state->buildings[1] |= 1<<i;
            case '1':
                state->buildings[0] |= 1<<i;
        }
    }

    state->workers[P1][0] = coords_to_spacei(&string[25]);
    state->workers[P1][1] = coords_to_spacei(&string[27]);
    state->workers[P2][0] = coords_to_spacei(&string[29]);
    state->workers[P2][1] = coords_to_spacei(&string[31]);

    state->turn = string[33] - '1';

    State_derive(state);
}


void State_to_string(const struct State *state, char *string)
{
    for (int i = 0; i < 25; i++) {
        string[i] = State_height_at(state, i) + '0';
    }

    spacei_to_coords(state->workers[P1][0], &string[25]);
    spacei_to_coords(state->workers[P1][1], &string[27]);
    spacei_to_coords(state->workers[P2][0], &string[29]);
    spacei_to_coords(state->workers[P2][1], &string[31]);

    string[33] = '1' + state->turn;
    string[34] = '\0';
}
