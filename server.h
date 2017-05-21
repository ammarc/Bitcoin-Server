#ifndef SERVER_H
#define SERVER_H

#define PONG "PONG"
#define PING "PING"
#define OKAY "OKAY"
#define ERRO "ERRO"
#define SOLN "SOLN"
#define WORK "WORK"

#include <stdbool.h>
#include "uint256.h"
#include "sha256.h"

void send_erro(BYTE error[40], int newsockfd);

bool check_sol(uint32_t difficulty, BYTE seed[64], uint64_t solution);

void work(uint32_t difficulty, BYTE seed[32], uint64_t start,
                                        uint8_t worker_count, int sockfd);

#endif