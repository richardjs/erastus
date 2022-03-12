#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <unistd.h>
#include "mcts.h"
#include "state.h"


enum Command {THINK, RANDOM, LIST_ACTIONS, ACT_AND_PRINT};


void think(const struct State *state)
{
    struct MCTSResults results;
    mcts(state, &results, NULL);

    char action_string[ACTION_STRING_SIZE];
    Action_to_string(&state->actions[results.actioni], action_string);
    printf("%s\n", action_string);

    fprintf(stderr, "score\t\t%f\n", results.score);

    fprintf(stderr, "time:\t\t%ld ms\n", results.stats.duration);
    fprintf(stderr, "iterations:\t%ld\n", results.stats.iterations);
    fprintf(stderr, "iters/s:\t%ld\n",
        results.stats.duration ?
            1000 * results.stats.iterations / results.stats.duration : 0);
    fprintf(stderr, "sim depth out:\t%.4g%%\n",
        results.stats.simulations ?
            100 * (float)results.stats.depth_outs / results.stats.simulations : 0);
    fprintf(stderr, "tree depth:\t%d\n", results.stats.tree_depth);
    fprintf(stderr, "tree size:\t%ld MiB\n",
        results.stats.tree_bytes / 1024 / 1024);

}


void check_state_string(char *state_string)
{
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


void list_actions(const struct State *state)
{
    fprintf(stderr, "%d actions\n", state->action_count);
    char action_string[ACTION_STRING_SIZE];
    for (int i = 0; i < state->action_count; i++) {
        Action_to_string(&state->actions[i], action_string);
        printf("%s\n", action_string);
    }
}


void act_and_print(const struct State *state, const struct Action *action)
{
    if (!State_check_action(state, action)) {
        fprintf(stderr, "Invalid action\n");
        exit(3);
    }

    struct State after = *state;
    State_act(&after, action);

    char state_string[STATE_STRING_SIZE];
    State_to_string(&after, state_string);
    printf("%s\n", state_string);

    State_print(&after);
}


void random_action(const struct State *state)
{
    struct State after = *state;
    State_act(&after, &state->actions[rand() % state->action_count]);

    char state_string[STATE_STRING_SIZE];
    State_to_string(&after, state_string);
    printf("%s\n", state_string);
}


int main(int argc, char *argv[])
{
    fprintf(stderr, "Erastus v.1a (built %s %s)\n", __DATE__, __TIME__);

    time_t seed = time(NULL);
    fprintf(stderr, "Seed:\t%ld\n", seed);
    srand(seed);

    enum Command command = THINK;
    struct Action action;
    int opt;
    while ((opt = getopt(argc, argv, "vlm:r")) != -1) {
        switch (opt) {
        case 'v':
            return 0;
        case 'l':
            command = LIST_ACTIONS;
            break;
        case 'm':
            command = ACT_AND_PRINT;
            Action_from_string(&action, optarg);
            break;
        case 'r':
            command = RANDOM;
        }
    }
    if (argc == optind) {
        fprintf(stderr, "Usage: %s [-l] [-m move] <state>\n", argv[0]);
        return 1;
    }


    check_state_string(argv[optind]);
    struct State state;
    State_from_string(&state, argv[optind]);


    switch (command) {
    case THINK:
        think(&state);
        break;

    case LIST_ACTIONS:
        list_actions(&state);
        break;

    case ACT_AND_PRINT:
        act_and_print(&state, &action);
        break;

    case RANDOM:
        random_action(&state);
        break;
    }


    return 0;
}
