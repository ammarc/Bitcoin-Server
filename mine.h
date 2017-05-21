#ifndef MINE_H
#define MINE_H

#include <stdbool.h>
#include "uint256.h"
#include "sha256.h"

struct soln_args
{
    uint32_t difficulty;
    BYTE seed[64];
    uint64_t solution;
};

struct work_args
{
    uint32_t difficulty;
    BYTE seed[64];
    uint64_t start;
    uint8_t worker_count;
    int sockfd;
};

bool check_sol(struct soln_args args);

void work(struct work_args args);

#endif