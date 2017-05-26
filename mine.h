/*  Project 2 for COMP30023: Computer Systems
 *  at the University of Melbourne
 *  Semester 1, 2017
 *  by: Ammar Ahmed
 *  Username: ammara
 */

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
    BYTE* target;
    int sockfd;
};

bool check_sol(struct soln_args args, BYTE* target);

void work(struct work_args args);

BYTE* find_target(uint32_t difficulty);

void set_abrt_true();

void set_abrt_false();
#endif