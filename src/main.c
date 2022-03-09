#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include "state.h"


enum Command {THINK, LIST_ACTIONS};


void check_state_string(char *state_string) {
    bool valid = true;
    for (int i = 0; i < STATE_STRING_SIZE - 2; i++) {
        if (!isalnum(state_string[i])) {
            valid = false;
            break;
        }
    }
    if (state_string[STATE_STRING_SIZE - 1] != '\0') {
        valid = false;
    }

    if (!valid) {
        fprintf(stderr, "Invalid state string: %s\n", state_string);
        exit(2);
    }
}


void list_actions(const struct State *state) {
    fprintf(stderr, "%d actions\n", state->action_count);
    char action_string[ACTION_STRING_SIZE];
    for (int i = 0; i < state->action_count; i++) {
        Action_to_string(&state->actions[i], action_string);
        printf("%s\n", action_string);
    }
}


int main(int argc, char *argv[]) {
    fprintf(stderr, "Erastus v.1a (built %s %s)\n", __DATE__, __TIME__);

    enum Command command = THINK;

    int opt;
    while ((opt = getopt(argc, argv, "vl")) != -1) {
        switch (opt) {
        case 'v':
            return 0;
        case 'l':
            command = LIST_ACTIONS;
            break;
        }
    }
    if (argc == optind) {
        fprintf(stderr, "Usage: %s [-l] <state>\n", argv[0]);
        return 1;
    }


    check_state_string(argv[optind]);
    struct State state;
    State_from_string(&state, argv[optind]);


    switch (command) {
    case LIST_ACTIONS:
        list_actions(&state);
        break;
    case THINK:
        break;
    }


    return 0;
}
