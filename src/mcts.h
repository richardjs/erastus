#ifndef MCTS_H
#define MCTS_H


#include "state.h"


#define DEFAULT_ITERATIONS 200000
#define DEFAULT_MAX_SIM_DEPTH 500
#define DEFAULT_UCTC 0.3
#define DEFAULT_SAVE_TREE false
#define DEFAULT_DOWN_PASS_RATE .99


struct Node {
    bool expanded;
    unsigned int visits;
    float value;

    struct Node **children;
    uint_fast16_t children_count;

    uint16_t depth;
};


struct MCTSOptions {
    uint64_t iterations;
    float uctc;
    float down_pass_rate;
    uint16_t max_sim_depth;
    bool save_tree;
};


struct MCTSStats {
    uint64_t iterations;
    uint64_t nodes;
    uint64_t tree_bytes;
    uint16_t tree_depth;
    uint32_t simulations;
    uint32_t early_sim_terminations;
    uint32_t tree_early_sim_terminations;
    uint32_t depth_outs;
    uint64_t duration;
};

struct MCTSResults {
    uint16_t actioni;
    float score;
    struct MCTSStats stats;
    struct Node nodes[MAX_ACTIONS];
    struct Node *tree;
};


void MCTSOptions_default(struct MCTSOptions*);


void mcts(const struct State*, struct MCTSResults*, const struct MCTSOptions*);


#endif
