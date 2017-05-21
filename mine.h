#ifndef MINE_H
#define MINE_H

#include <stdbool.h>
#include "uint256.h"
#include "sha256.h"

bool check_sol(uint32_t difficulty, BYTE seed[64], uint64_t solution);

void work(uint32_t difficulty, BYTE seed[32], uint64_t start,
                                        uint8_t worker_count, int sockfd);

#endif