#include "mcts.h"

#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/time.h>


// store these globally so we don't have to pass them around
static struct MCTSOptions options;
static struct MCTSResults *results;


/**
 * mallocs, checks for null, and increases results.stats.tree_bytes
 */
void *mctsmalloc(size_t size)
{
    void *ptr = malloc(size);
    if (ptr == NULL) {
        fprintf(stderr, "ERROR: failure to malloc in MCTS\n");
        exit(1);
    }
    results->stats.tree_bytes += size;
    return ptr;
}


void MCTSOptions_default(struct MCTSOptions *o)
{
        o->iterations = DEFAULT_ITERATIONS;
        o->uctc = DEFAULT_UCTC;
        o->down_pass_rate = DEFAULT_DOWN_PASS_RATE;
        o->max_sim_depth = DEFAULT_MAX_SIM_DEPTH;
        o->save_tree = DEFAULT_SAVE_TREE;
}


void Node_init(struct Node *node, uint8_t depth)
{
    node->expanded = false;
    node->visits = 0;
    node->value = 0;
    // TODO we probably could pass this around mcts() and iterate()
    // instead of storing it here
    node->depth = depth;

    results->stats.nodes++;
    if (depth > results->stats.tree_depth) {
        results->stats.tree_depth = depth;
    }
}


/**
 * allocates space for children pointers and the child nodes themselves,
 * and calls Node_init on each child
 */
void Node_expand(struct Node *node, const struct State *state)
{
    node->children_count = state->action_count;
    node->children = mctsmalloc(sizeof(struct Node*) * node->children_count);

    for (int i = 0; i < state->action_count; i++) {
        node->children[i] = mctsmalloc(sizeof(struct Node));
        Node_init(node->children[i], node->depth+1);
    }

    node->expanded = true;
}


/**
 * frees the node's children along with the node
 */
void Node_free(struct Node *node)
{
    if (node->expanded) {
        for (int i = 0; i < node->children_count; i++) {
            Node_free(node->children[i]);
        }
        free(node->children);
    }

    free(node);
}


/**
 * simulates play (in place) on a state, stopping at game end or
 * MAX_SIM_DEPTH, and returns 1.0 if the initial state won, -1.0 if it
 * lost, and 0.0 on a draw or depth out
 */
float simulate(struct State *state)
{
    results->stats.simulations++;

    enum Player turn = state->turn;

    int depth = 0;
    while (state->action_count) {
        if (depth++ > options.max_sim_depth) {
            results->stats.depth_outs++;
            return 0.0;
        }

        if (State_unstoppable_win(state)) {
            results->stats.early_sim_terminations++;
            return turn == state->turn ? 1.0 : -1.0;
        }

        // If there's a win, it will be placed in actions[0] during action derivation
        if (state->actions[0].build == WIN) {
            return turn == state->turn ? 1.0 : -1.0;
        } else {
            int tries = 0;
            struct Action action;
            while (tries < 100) {
                tries++;

                action = state->actions[rand() % state->action_count];

                uint_fast8_t source_height = State_height_at(state, action.source);
                uint_fast8_t dest_height = State_height_at(state, action.dest);
                if (dest_height < source_height) {
                    if ((rand() / (float) RAND_MAX) < options.down_pass_rate) {
                        continue;
                    }
                }

                break;
            }

            State_act(state, &action);

        }
    }

    // It doesn't matter if action_count==0 due to winning or no legal
    // moves; the current turn loses. If it's a win, the winner just
    // acted, so it's the loser's turn (but the game is over).
    return turn == state->turn ? -1.0 : 1.0;
}


/**
 * single MCTS iteration: recursively walk down tree with state
 * (choosing promising children), simulate when we get to the end of the
 * tree, and update visited nodes with the results
 */
float iterate(struct Node *root, struct State *state)
{
    // game-terminal node
    if (state->action_count == 0) {
        root->visits++;
        root->value += -1.0;
        return -1.0;
    }
    if (state->actions[0].build == WIN) {
        root->visits++;
        root->value += 1.0;
        return 1.0;
    }
    if (State_unstoppable_win(state)) {
        //State_print(state);
        root->visits++;
        root->value += 1.0;
        results->stats.tree_early_sim_terminations++;
        return 1.0;
    }

    if (!root->expanded) {
        Node_expand(root, state);
    }

    if (root->visits == 0) {
        float score = simulate(state);

        root->visits++;
        root->value += score;
        return score;
    }

    int childi = 0;
    float best_uct = -INFINITY;
    for (int i = 0; i < state->action_count; i++) {
        if (root->children[i]->visits == 0) {
            childi = i;
            break;
        }

        float uct =
            -1*root->children[i]->value/root->children[i]->visits
            + options.uctc*sqrtf(logf(root->visits)/root->children[i]->visits);

        if (uct >= best_uct) {
            best_uct = uct;
            childi = i;
        }
    }

    struct Node *child = root->children[childi];
    State_act(state, &state->actions[childi]);

    float score = -1*iterate(child, state);

    root->visits++;
    root->value += score;
    return score;
}


void mcts(const struct State *state, struct MCTSResults *r, const struct MCTSOptions *o)
{
    results = r;
    memset(results, 0, sizeof(struct MCTSResults));

    if (o == NULL) {
        MCTSOptions_default(&options);
    } else {
        options = *o;
    }

    if (state->action_count == 0) {
        fprintf(stderr, "Can't run MCTS on state with no actions\n");
        return;
    }

    struct Node *root = mctsmalloc(sizeof(struct Node));
    Node_init(root, 0);
    Node_expand(root, state);

    struct timeval start;
    gettimeofday(&start, NULL);

    for (int i = 0; i < options.iterations; i++) {
        struct State s = *state;
        iterate(root, &s);
        results->stats.iterations++;
    }

    struct timeval end;
    gettimeofday(&end, NULL);
    results->stats.duration = (end.tv_sec - start.tv_sec)*1000 + (end.tv_usec - start.tv_usec)/1000;

    results->score = -INFINITY;
    for (int i = 0; i < state->action_count; i++) {
        float score = -1 * root->children[i]->value / root->children[i]->visits;

        if (score >= results->score) {
            results->score = score;
            results->actioni = i;
        }
    }

    for (int i = 0; i < state->action_count; i++) {
        results->nodes[i] = *root->children[i];
    }

    if (options.save_tree) {
        results->tree = root;
    } else {
        Node_free(root);
        results->tree = NULL;
    }
}
