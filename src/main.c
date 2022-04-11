#include <ctype.h>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/time.h>
#include <sys/wait.h>
#include <time.h>
#include <unistd.h>
#include "mcts.h"
#include "minimax.h"
#include "state.h"


enum Command {THINK, RANDOM, LIST_ACTIONS, ACT_AND_PRINT};


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
    if (State_check_win(state)) {
        fprintf(stderr, "state is a win; no actions\n");
        return;
    }

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
    struct Action action = state->actions[rand() % state->action_count];

    char action_string[ACTION_STRING_SIZE];
    Action_to_string(&action, action_string);
    printf("%s\n", action_string);

    struct State after = *state;
    State_act(&after, &action);
    if (after.action_count == 0) {
        fprintf(stderr, "no more actions");
    }
}


int main(int argc, char *argv[])
{
    fprintf(stderr, "Erastus v.7.2 (built %s %s)\n", __DATE__, __TIME__);

    time_t seed = time(NULL);
    srand(seed);

    enum Command command = THINK;
    int workers = 1;

    struct MCTSOptions options;
    MCTSOptions_default(&options);

    struct Action action;

    int opt;
    while ((opt = getopt(argc, argv, "vlm:ri:c:d:w:")) != -1) {
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
            break;
        case 'i':
            options.iterations = atoi(optarg);
            break;
        case 'c':
            options.uctc = atof(optarg);
            break;
        case 'd':
            options.down_pass_rate = atof(optarg);
            break;
        case 'w':
            workers = atoi(optarg);
            break;
        }
    }
    if (argc == optind) {
        fprintf(stderr, "Usage: %s [-l] [-m move] <state>\n", argv[0]);
        return 1;
    }


    check_state_string(argv[optind]);
    struct State state;
    State_from_string(&state, argv[optind]);
    fprintf(stderr, "input:\t\t%s\n", argv[optind]);
    State_print(&state);

    switch (command) {
    case LIST_ACTIONS:
        list_actions(&state);
        return 0;
        break;

    case ACT_AND_PRINT:
        act_and_print(&state, &action);
        return 0;
        break;

    case RANDOM:
        random_action(&state);
        return 0;
        break;
    }

    if (state.actions[0].build == WIN) {
        char action_string[ACTION_STRING_SIZE];
        Action_to_string(&state.actions[0], action_string);
        printf("%s\n", action_string);
        fprintf(stderr, "taking win\n");
        return 0;
    }

    if (State_is_start_phase(&state)) {
        fprintf(stderr, "start phase; using hardcoded action\n");
        struct Action actions[4];
        for (int i = 0; i < 4; i++) {
            actions[i].build = PLACE;
        }
        actions[0].source = 7;
        actions[0].dest = 17;
        actions[1].source = 11;
        actions[1].dest = 13;
        actions[2].source = 6;
        actions[2].dest = 18;
        actions[3].source = 8;
        actions[3].dest = 16;

        for (int i = 0; i < 4; i++) {
            if (State_check_action(&state, &actions[i])) {
                char action_string[ACTION_STRING_SIZE];
                Action_to_string(&actions[i], action_string);
                printf("%s\n", action_string);
                return 0;
            }
        }
    }

    if (State_unstoppable_win(&state)) {
        fprintf(stderr, "unstoppable win detected; using minimax\n");

        struct MinimaxResults results;
        struct MinimaxOptions options;
        options.depth = 3;

        minimax(&state, &results, &options);

        char action_string[ACTION_STRING_SIZE];
        Action_to_string(&state.actions[results.actioni], action_string);
        printf("%s\n", action_string);

        fprintf(stderr, "action:\t%s\n", action_string);
        fprintf(stderr, "score:\t%f\n", results.score);

        struct State after = state;
        State_act(&after, &state.actions[results.actioni]);
        State_print(&after);

        return 0;
    }

    int pipefd[2];
    pipe(pipefd);

    for (int i = 0; i < workers; i++) {
        srand(rand());
        if (fork() > 0) {
            continue;
        }
        struct MCTSResults results;
        mcts(&state, &results, &options);
        write(pipefd[1], &results, sizeof(struct MCTSResults));
        exit(0);
    }

    struct MCTSResults results;
    memset(&results, 0, sizeof(struct MCTSResults));

    struct timeval start;
    gettimeofday(&start, NULL);

    for (int i = 0; i < workers; i++) {
        wait(NULL);

        struct MCTSResults worker_results;
        read(pipefd[0], &worker_results, sizeof(struct MCTSResults));

        for (int j = 0; j < state.action_count; j++) {
            results.nodes[j].visits += worker_results.nodes[j].visits;
            results.nodes[j].value += worker_results.nodes[j].value;
        }

        results.stats.iterations += worker_results.stats.iterations;
        results.stats.nodes += worker_results.stats.nodes;
        results.stats.tree_bytes += worker_results.stats.tree_bytes;
        results.stats.simulations += worker_results.stats.simulations;
    }

    struct timeval end;
    gettimeofday(&end, NULL);
    results.stats.duration = (end.tv_sec - start.tv_sec)*1000 + (end.tv_usec - start.tv_usec)/1000;

    results.score = -INFINITY;
    for (int i = 0; i < state.action_count; i++) {
        float score = -1 * results.nodes[i].value / results.nodes[i].visits;

        if (score >= results.score) {
            results.score = score;
            results.actioni = i;
        }
    }

    char action_string[ACTION_STRING_SIZE];
    Action_to_string(&state.actions[results.actioni], action_string);
    printf("%s\n", action_string);

    fprintf(stderr, "action:\t\t%s\n", action_string);
    fprintf(stderr, "score:\t\t%.2f\n", results.score);

    fprintf(stderr, "iterations:\t%ld\n", results.stats.iterations);
    fprintf(stderr, "workers:\t%d\n", workers);
    fprintf(stderr, "time:\t\t%ld ms\n", results.stats.duration);
    fprintf(stderr, "iters/s:\t%ld\n",
        results.stats.duration ?
            1000 * results.stats.iterations / results.stats.duration : 0);
    fprintf(stderr, "tree size:\t%ld MiB\n",
        results.stats.tree_bytes / 1024 / 1024);

    struct State after = state;
    State_act(&after, &state.actions[results.actioni]);
    State_print(&after);

    return 0;
}
