#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/time.h>
#include "state.h"


#define NUM_GAMES 100000


struct BenchmarkData {
    uint64_t calls;
    uint64_t secs;
    uint64_t usecs;
};


float BenchmarkData_calculate_usecs(const struct BenchmarkData *data)
{
    return (data->secs*1000000 + data->usecs) / (float)data->calls;
}


int main()
{
    time_t seed = time(NULL);
    srand(seed);

    struct State state;
    struct Action action;

    struct BenchmarkData act_data;
    memset(&act_data, 0, sizeof(struct BenchmarkData));

    struct timeval start;
    struct timeval end;

    printf("Playing games...\n");
    for (int i = 0; i < NUM_GAMES; i++) {
        if (i % (NUM_GAMES/10) == 0) {
            putchar('.');
            fflush(stdout);
        }
        State_new(&state);

        while (state.action_count) {
            action = state.actions[rand() % state.action_count];

            if (action.build == WIN) {
                break;
            }

            gettimeofday(&start, NULL);
            State_act(&state, &action);
            gettimeofday(&end, NULL);

            act_data.calls++;
            act_data.secs += end.tv_sec - start.tv_sec;
            act_data.usecs += end.tv_usec - start.tv_usec;
        }
    }

    printf("\nState_act:\t%f\n", BenchmarkData_calculate_usecs(&act_data));

    return 0;
}
